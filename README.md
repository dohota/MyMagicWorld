有了vcpkg.json：vcpkg 自动进入 manifest 模式, VS Code + CMake Tools 会自动配合：
✅ 不用手动执行 vcpkg install xxx
✅ 只改 vcpkg.json
✅ CMake / VS Code 会自动下载、自动删除

vcpkg 所管理的库叫 ports
每个库都有一个目录和配置文件（portfile.cmake），指定：
源码 URL（通常是 GitHub、官方 release 压缩包）
依赖的其他库
构建选项（CMake、编译宏、patch 等）

该项目配置：目前用的是m4系列的macos，ide用vscode（装了clangd和cmake的插件）
brew下载了cmake，ninja，vcpkg，然后项目好像就能自动识别了
点击左下角的启动按钮，就能自动启动项目了

vscode最左侧有debug的按钮，可以用来调试，设置断点等
可以直接在控制台输入：/Users/karl/Documents/programming/c/FirstWorld/build/MyWorld来启动该游戏
#### 目前项目设想
a.ComponentPool 天然适合 SIMD / 性能优化
1.选最小 pool：在所有 Ts... 的 component pool 中，选一个 entity 数量最少的 pool 作为外层循环
如 view< Position, Velocity, Camera>() 应该先 遍历 Camera pool
2 . AoS改为SoA：cache 命中 ✅ SIMD ✅ 线性访问 ✅
3 . 去掉 unordered_map ，改为 dense type id （把组件类型映射成一个小整数），
查找————O(1) 数组，cache	连续，可 bitmask
4 . archetype = 组件集合的“形态”， 一个组件集合的签名：
Entity 创建，组件集合确定 ，Entity 被放入某个 archetype chunk，System 只遍历自己关心的 archetype

b. 不知道之后要不要把component manager与entity manager分离，但目前来看没必要
c.之前是多个系统执行发生冲突了。我把input- move-collision- render改为input- collision-move- render 后
碰撞系统里的函数大都能正常执行了，可是碰撞的画面还是没看到。所以我就把move合并进了collision系统
（因为这两个系统同时都修改了position），之后不知道要不要再解耦合
d.每个系统是不是就写成一个函数就行，这样也方便调度吧！因为system理论上不需要什么初始状态，或者说system manager可以帮忙管理其初始状态
加上 scheduler，system dependency graph

事件总线让system之间只通过“事件”通信 Job Graph： 把各种system的update方法拆成很多极小的任务（Job），用“依赖关系图”而不是“调用顺序”来驱动执行。如：更新 128 个 Transform
传统模型（单线程思维）：函数调用 + Tick：顺序是“人为规定的”，大量时间在 等，很难自动并行改一个模块就可能破全局顺序
Scheduler（调度器）负责：找到“没有依赖”的 Job，分配到线程池，动态负载均衡，尽量减少线程空转。即job graph基于线程池之上，线程池又基于os给的线程/进程
 /shader /model：放着色器文件，.png .ogg 

#### bug （可以写进github的issue里）
移动方向bug：摄像机没问题，但是人物wasd移动不能按照人物面朝的方向进行移动
碰撞箱bug：比如从上到下进入方块，却只和底面碰撞。估计是位置错位之类的问题 
（overlapsOnOtherAxes 从来没被调用，但好像不是什么问题）
渲染剔除bug：不知道为啥，剔除范围太大了，所以我就注释掉了
增加删除方块bug：不是根据raycast视线来删除的（好像是根据固定顺序删除），增加方块也不可见，但是肯定是增加了

#### v1.5.9
感觉之前的段错误是 即时制事件队列导致的，所以现在新增了新的事件队列，在每一tick的末尾修改实体
现在增加方块好像看不见了，删除方块能看到，但是删的太快了，位置也不对。而且event bus都没派上用处，像一个累赘
#### v1.5.10
现在增加方块，删除方块 的速度正常了
#### v1.5.11
方块系统还是有bug，先不管了
## mc Java版pre-Classic （north创造mc的第一周）：和我现在写的内容差不多
## Classic 初始版本：有较大地图，少量方块种类（都有材质纹理），只有飞行模式，选中方块有高亮，有简单的多人联机功能，简易ui
#### v1.6.0 : 
用bgfx替代opengl，只改render system和world.cpp的代码就行了。bgfx类似opengl，但是流程更加现代化，可以参考其源代码中给的渲染范例
要是感觉不行就再换用filament
#### v1.6.1:




加入openal声音库，基本只播放.ogg文件。ui用imgui。物理引擎等别的库按需引入
合适的话可以加入网络模块（如entt，raknet，boost.asio）
#### 可以试试sdl3或者glfw，但性能也差不多了多少；仿照minetest的代码！
未实现：
双击空格 切换飞行模式和地面模式（跳跃/重力系统）
实现简单的chunk加载与删除，无限地形
编译期间将头文件里的数值赋值给变量，初始化组件在编译期间就完成
最好是等有了稳定的chunk系统了之后，再加入网络模块（像mc一样），并且内置一个内网穿透的工具

#### cworld_forge分支
v 0.0.x：学习使用the forge，性能很好，适合3a大作，也支持全平台，包括游戏主机平台。sokol库太简陋，暂时不需要用
the forge源代码以我的能力，不需要改了。the forge还依赖很多第三方库，那些库大都以二进制形式提供（已经编译好了的库）
当然，the forge和其依赖的第三方库，如果有bug的话，还是需要跟原作者反应，在issue里提出
我做游戏引擎/游戏的时候，需要把the forge的源代码和 游戏的源代码 放在一起编译，最后链接在一起
/theforge：放forge源代码（编译为被调用的静态库），/src放我游戏的源代码（最后编译为可执行文件）
可以写cmake脚本编译二者，也可以用lua脚本什么的，
等项目超过十万行更可以自制构建工具（自制构建工具还要有能在编译期报错的本领）

v 0.1.x: 可以引入openal，和别的网络库。它们可以写在vcpkg.json里，也可以自己下载它们的二进制形式
the forge本身的声音库只有薄薄的一层，网络库更是基本没有
the forge的教程比较少，但可以去discord上找。最好的教程就是阅读其源代码，因为这个库是给工程师用的
The Forge 的重心是：GPU / CPU / 内存 / 多线程 / 跨平台，别的就自己实现/引入第三方库。the forge很克制的使用std标准库，但不像ue那样完全不用
## 文件夹规范：
build：cmake/vcpkg自动构建的，编译产物，中间文件 
src：主要的游戏源代码：分引擎（纯客户端），游戏本体（客户端+服务端）
test：单元测试，各种测试，如cmake的ctest
docs（用md文件不要用二进制文件）：设计文档，架构说明，API 文档，教程。可以与wiki里的内容互相配合 
.github：GitHub 自动识别的配置目录————控制 CI、Issue 模板、PR 模板 
assets：各种声音，贴图等 
tools：内部工具，转换器，打包器。可以放入自己的构建工具和自己的版本控制工具
examples：示例 
lib：第三方库，预编译库，子模块 ——————————可以放the forge的源代码
include（C/C++ 常见）：公共头文件，对外 API 
scripts：构建脚本，自动化脚本，工具脚本 
config：配置文件，如JSON / YAML / TOML，可以用.hpp配置 初始化组件
