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

#### v1.3.2
先不要分entity manager，component manager，system manager，world manager，全耦合在world里比较方便
#### v1.3.3
先试试渲染系统能不能成功（固定摄像机）
#### 1.3.3 b
编译+链接成功，但没有画面显示

未来实现：
    /util：
        ├── aabb.c
        ├── raycast.c
        ├── timer.c
        ├── config.c 各种全局配置

    /core：
    shader.xx：OpenGL着色器等配置
    # 有关窗口，输入，声音，渲染的底层库，之后这里可以放sokol文件
