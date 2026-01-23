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

## 项目构思：
先用sdl2+opengl，然后可以试试加上openal和raknet/boost.asio,ui就使用imgui，物理引擎也可以抄开源的
再试试换用sdl3/glfw是什么效果（可以仿照minetest的代码）
接下来再试试用bgfx/filament换掉opengl，然后用the forge来写（性能很好），最后全都自己手写，可以参考sokol库
构建方面暂定：cmake+ninja+vcpkg，等项目超过十万行再考虑别的

底层和操作系统/硬件打交道的代码用c语言，上层用c++ ecs架构，其中chunk会特殊一点，一堆方块视为一个entity

#### v 1.4.0
第一个正式版本！
可以wasd移动摄像机，移动鼠标来实现视角的移动————————但是有bug，更像是整个世界的移动
————渲染方面 又绿又黑的，可能是opengl没弄好
暂时不分entity manager，component manager，system manager，world manager，全耦合在world里比较方便
各个system以头文件，内联函数形式提供
#### v 1.4.1
修复了渲染的问题————边框渲染代码不太好
#### v 1.4.2
暂时比较难 修复摄像机的问题。涉及到很多数学知识，接下来尽量就让人物在地面上行走吧
#### v 1.4.3
慢慢解耦合：
system尽量都写成类
并且设置system manager
#### v 1.4.4
创造销毁实体，增加删除实体组件：entity manager
但是出了点问题，所以接下来我要引入类似一个component manager（管理组件的数据结构）
#### v 1.4.5
第二个正式版本！
entity manager已经有manage component的功能了，接下来就只要ComponentPool来管理组件的数据结构了
ComponentPool 天然适合 SIMD / 性能优化
#### v 1.4.6
有简单的随机地形，除了摄像机外没啥bug
#### v1.4.7
实现了十字准星
修复摄像机问题：
鼠标仅转动视角，不影响移动方向和移动速度
wasd人物向视角前后左右移动,space shift人物上下移动
## 不过wasd方向还是有点问题！
#### v1.5.0
实现aabb碰撞箱.但是碰撞方面有bug，不碰撞的时候人物也无法移动
#### v1.5.1
稍微改进了碰撞箱，但还是有bug
#### v1.5.2
现在猜测可能是多个系统执行发生冲突了，比如之前是input- move-collision- render
现在改为input- collision-move- render 后，碰撞系统里的函数大都能正常执行了，可是碰撞的画面还是没看到
结果是这两个系统同时都修改了position，所以这两个系统需要合并为同一个系统，或者是之后用事件总线解决？
所以：暂时解决了碰撞系统的问题，但是还是有点bug：
## 比如从上到下进入方块，却只和底面碰撞
（overlapsOnOtherAxes 从来没被调用，但好像不是什么问题）
#### v1.5.3
聪明渲染：剔除看不见的面——————只改了render system
但是目前有bug（也只用改动render system）
#### v1.5.4
## 暂时修复不了上面渲染剔除的bug，所以把渲染剔除的代码注释掉了
#### v1.5.5 b
初步实现了raycast 选中某个方块
#### v1.5.6
现在理论上可以左键放置方块，右键删除方块了（通过raycast system实现），但还是有一些bug：
#### 1.能创建/删除方块，但是位置明显不对.之后改进了位置稍微没那么离谱了，但是增加方块又有点看不见了
2.因为暂时没有事件总线，所以代码可能比较耦合：会出现一边遍历组件的同时，同时又在删除组件
（研究一下怎么让input表示意图，怎么发事件给别的系统呢？？创造/销毁实体要不要也搞成一个系统？？只接受事件）
#### v1.5.7
vscode最左侧有debug的按钮，可以用来调试，设置断点等
可以直接在控制台输入：/Users/karl/Documents/programming/c/FirstWorld/build/MyWorld来启动该游戏
#### v1.5.8
写了个事件总线，没有想象中的那么困难
每个系统是不是就写成一个函数就行，这样也方便调度吧！因为system理论上不需要什么初始状态，或者说system manager可以帮忙管理其初始状态
#### bug：鼠标左键/右键，则出现段错误。 让接受事件不每帧调用————还是不行
那就先无视这个bug，实现后面的功能

双击空格 切换飞行模式和地面模式（跳跃/重力系统）
实现简单的chunk加载与删除，无限地形
编译期间将头文件里的数值赋值给变量，初始化组件在编译期间就完成
🔥 继续：
view<T...>() 选最小 pool（O(min)）
SoA / SIMD MoveSystem
🧠 进阶：
去掉 unordered_map → dense type id
archetype / chunk layout
🎮 工程化：
scheduler
system dependency graph
