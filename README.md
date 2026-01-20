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



🔥 继续：

view<T...>() 选最小 pool（O(min)）

SoA / SIMD MoveSystem

🧠 进阶：

去掉 unordered_map → dense type id

archetype / chunk layout

🎮 工程化：

scheduler

system dependency graph

管理不同world：world manager，这个暂时不需要
尽量让人物在地面上走，仿照初代mc飞行模式和生存模式，这样摄像机就会好弄一点
