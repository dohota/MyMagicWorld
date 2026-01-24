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

vscode最左侧有debug的按钮，可以用来调试，设置断点等
可以直接在控制台输入：/Users/karl/Documents/programming/c/FirstWorld/build/MyWorld来启动该游戏

#### 目前项目设想
ComponentPool 天然适合 SIMD / 性能优化

之前是多个系统执行发生冲突了。我把input- move-collision- render改为input- collision-move- render 后
碰撞系统里的函数大都能正常执行了，可是碰撞的画面还是没看到。所以我就把move合并进了collision系统
（因为这两个系统同时都修改了position），之后不知道要不要再解耦合

每个系统是不是就写成一个函数就行，这样也方便调度吧！因为system理论上不需要什么初始状态，或者说system manager可以帮忙管理其初始状态

#### bug
移动方向bug：摄像机没问题，但是人物wasd移动不能按照人物面朝的方向进行移动
碰撞箱bug：比如从上到下进入方块，却只和底面碰撞。估计是位置错位之类的问题 
（overlapsOnOtherAxes 从来没被调用，但好像不是什么问题）
渲染剔除bug：不知道为啥，剔除范围太大了，所以我就注释掉了
增加删除方块bug：

#### v1.5.9
感觉之前的段错误是 即时制事件队列导致的，所以现在新增了新的事件队列，在每一tick的末尾修改实体
现在增加方块好像看不见了，删除方块能看到，但是删的太快了，位置也不对。而且event bus都没派上用处，像一个累赘




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
