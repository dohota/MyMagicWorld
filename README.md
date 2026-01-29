## cworld分支总结：

#### 关于项目构建
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
### 目前项目设想
#### a.
ComponentPool 天然适合 SIMD / 性能优化
#### 1.
选最小 pool：在所有 Ts... 的 component pool 中，选一个 entity 数量最少的 pool 作为外层循环
如 view< Position, Velocity, Camera>() 应该先 遍历 Camera pool
#### 2.
AoS改为SoA：cache 命中 ✅ SIMD ✅ 线性访问 ✅
#### 3 . 
去掉 unordered_map ，改为 dense type id （把组件类型映射成一个小整数），
查找————O(1) 数组，cache	连续，可 bitmask
#### 4 . 
archetype = 组件集合的“形态”， 一个组件集合的签名：
Entity 创建，组件集合确定 ，Entity 被放入某个 archetype chunk，System 只遍历自己关心的 archetype
#### b. 
不知道之后要不要把component manager与entity manager分离，但目前来看没必要
#### c.
之前是多个系统执行发生冲突了。我把input- move-collision- render改为input- collision-move- render 后
碰撞系统里的函数大都能正常执行了，可是碰撞的画面还是没看到。所以我就把move合并进了collision系统
（因为这两个系统同时都修改了position），之后不知道要不要再解耦合
#### d.
每个系统是不是就写成一个函数就行，这样也方便调度吧！因为system理论上不需要什么初始状态，或者说system manager可以帮忙管理其初始状态
加上 scheduler，system dependency graph

事件总线让system之间只通过“事件”通信 Job Graph： 把各种system的update方法拆成很多极小的任务（Job），用“依赖关系图”而不是“调用顺序”来驱动执行
如：更新 128 个 Transform
传统模型（单线程思维）：函数调用 + Tick：顺序是“人为规定的”，大量时间在 等，很难自动并行改一个模块就可能破全局顺序
Scheduler（调度器）负责：找到“没有依赖”的 Job，分配到线程池，动态负载均衡，尽量减少线程空转。即job graph基于线程池之上，线程池又基于os给的线程/进程

#### bug （可写入github的issue里）
移动方向bug：摄像机没问题，但是人物wasd移动不能按照人物面朝的方向进行移动
碰撞箱bug：比如从上到下进入方块，却只和底面碰撞。估计是位置错位之类的问题 
（overlapsOnOtherAxes 从来没被调用，但好像不是什么问题）
渲染剔除bug：不知道为啥，剔除范围太大了，所以我就注释掉了
增加删除方块bug：不是根据raycast视线来删除的（好像是根据固定顺序删除），增加方块也不可见，但是肯定是增加了
（感觉之前的段错误是 即时制事件队列导致的，所以现在新增了新的事件队列，在每一tick的末尾修改实体
现在增加方块好像看不见了，删除方块能看到，但是删的太快了，位置也不对。而且event bus都没派上用处，像一个累赘）
后期（v1.6.0-- v1.6.2 c）用bgfx代替opengl进行渲染，只改render system和world.cpp的代码就行
但是出现了bug，bgfx无论如何都不能初始化
原因应该是vcpkg下载bgfx的时候没有对m4的MacBook进行很好的适配，导致链接错误
以后需要手动编译bgfx！当然最好还是抛弃bgfx，用the forge

#### 计划实现但未实现的功能：
可以试试sdl3或者glfw，但性能也差不多了多少；仿照minetest的代码！
双击空格 切换飞行模式和地面模式（跳跃/重力系统）
实现简单的chunk加载与删除，无限地形
编译期间将头文件里的数值赋值给变量，初始化组件在编译期间就完成
ui用imgui。物理引擎等别的库按需引入
最好是等有了稳定的chunk系统了之后，再加入网络模块（像mc一样,但是这里可用entt，raknet，boost.asio等）

## 理想的文件夹规范：
build：cmake/vcpkg自动构建的，编译产物，中间文件 
src：主要的游戏源代码：分引擎（纯客户端），游戏本体（客户端+服务端）
test：单元测试，各种测试，如cmake的ctest
docs（用md文件不要用二进制文件）：设计文档，架构说明，API 文档，教程。可以与wiki里的内容互相配合 
.github：GitHub 自动识别的配置目录————控制 CI、Issue 模板、PR 模板 
assets：图片，音乐，字体等。图片一律用.png，字体.ttf，音乐.ogg 
tools：内部工具，转换器，打包器。可以放入自己的构建工具和自己的版本控制工具
examples：示例 
lib：第三方库，预编译库，子模块 ——————————可以放the forge的源代码
include（C/C++ 常见）：公共头文件，对外 API 
scripts：构建脚本，自动化脚本，工具脚本 
config：配置文件，如JSON / YAML / TOML，可以用.hpp配置 初始化组件
src/util：自己实现类似的std标准库，自己实现内存池（减少对raii的依赖），多线程
各种数据类型都可以自己封装一下（比如判断浮点数相等），自己实现运行时异常的处理，日志功能（借鉴spdlog）
要自己实现字符串，默认是utf-8，并且实现多语言国际化。可以引用stb单头文件库

## 构建：
项目超过十万行，可以试着自己研发构建工具
目前构建系统是cmake+ninja+clang，ide用vscode，跨平台很方便
在Windows上也可以尝试cmake+msvc。反正核心是cmake
CMakeLists.txt 是“蓝图”，cmake 命令是“建造工厂”，生成 Makefile/VS 工程，最后让系统去构建
CMake Tools插件：自动检测 CMakeLists.txt，管理 build 文件夹，生成构建系统（Makefile / Ninja / VS 工程），
编译、运行程序，支持调试（断点、单步）
### build 文件夹里通常放哪些东西？
#### 1.生成的构建系统文件
这些是 CMake 根据 CMakeLists.txt 生成的文件，用于告诉编译器怎么编译、链接：
Makefile或Ninja（如果你用 Ninja 生成器）或VisualStudio工程文件
CMakeCache.txt：缓存变量和路径信息
CMakeFiles/：CMake 的内部目录，存储依赖关系、目标信息等
#### 为什么要放这里？
让编译器可以知道每个 target 怎么编译
CMake 可以缓存信息，避免每次都重新解析 CMakeLists.txt

#### 2.中间编译文件
编译源文件时产生的临时文件，例如：
.o / .obj 文件（目标文件）
.a / .lib 文件（静态库）
.so / .dll / .dylib 文件（动态库）
#### 为什么要放这里？
防止源码目录被临时文件污染
支持多配置构建（Debug/Release）共存

#### 3.最终输出文件
根据 CMakeLists.txt 定义的 target，最终可执行文件或库也会放在 build 目录中，例如：
build/MyApp（Linux/macOS 可执行文件）
build/Debug/MyApp.exe（Windows Debug 可执行文件）
#### 为什么要放这里？
与源码分离，可以同时保留不同配置的输出（Debug 和 Release）

#### 4.CMake 特殊缓存文件
CMakeCache.txt：存储用户设置和路径
cmake_install.cmake：安装规则脚本
CMakeFiles/ 目录内部还会有：构建依赖关系，每个 target 的编译信息，内部临时文件

### 第三方库的位置
#### (a) 系统自带库 / 全局安装库
安装在系统目录，如：
Linux: /usr/lib、/usr/include
macOS: /usr/local/lib、/usr/local/include
Windows: C:\Program Files\...
CMake 通过 find_package 或 find_library 找到它们
你不需要放到 build 里，CMake 会直接链接系统路径下的库

#### (b) vcpkg 下载的第三方库
vcpkg 有自己的安装目录，如：
C:\vcpkg\installed\x64-windows 或 /vcpkg/installed/x64-linux

当你在 CMakeLists.txt 里加上：find_package(OpenCV REQUIRED)
并且设置：set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
CMake 会自动知道去 vcpkg 安装目录查找头文件和库文件

#### （c）随便某个位置
写相对路径/绝对路径让cmake知道位置，如：
头文件路径
target_include_directories(MyApp PRIVATE "D:/libs/mylib/include")
库文件路径
target_link_libraries(MyApp PRIVATE "D:/libs/mylib/lib/mylib.lib")

#### CMake Toolchain file：
是一个 .cmake 文件，用来告诉 CMake 使用哪个编译器、工具链和平台设置。是 跨平台构建的关键

## my_forge分支
#### The-Forge
the forge性能很好，适合3a大作，也支持全平台，包括游戏主机平台。与其相比，sokol库就太简陋了，暂时不需要用
用cmake构建the forge很难，而且the forge官方没有用cmake
我在Windows11上用紫色的vs试着编译了一下，结果显示windows sdk不匹配

the forge的教程比较少，但可以去discord上找。最好的教程就是阅读其源代码，因为这个库是给工程师用的
The Forge 的重心是：GPU / CPU / 内存 / 多线程 / 跨平台，别的就自己实现/引入第三方库。the forge很克制的使用std标准库，但不像ue那样完全不用
#### The-Forge-Lite
https://github.com/boberfly/The-Forge-Lite?utm_source=chatgpt.com
也可以试试这个精简版的the forge，少了一些第三方库，gpu渲染方面不比原版差，cpu内存性能需要自己实现优化（如异步加载什么的）
总的来说，如果专门为超大型mc而优化，那性能甚至可以比原版the forge还要好
但是它支持的平台会少一点，好像不支持opengl。不过在cmake里可以自己选择编译哪些平台

最好把the forge代码编译为静态库，放入/theforge文件夹，/src放我游戏的源代码
将二者放在一起编译，最后链接在一起，即为可执行文件
#### 音乐
the forge本身的声音库只有薄薄的一层
借鉴openal（可能不是完美适应现代ecs），miniaudio（单头文件，不到一万行，但是非常适合超大ecs mc）
#### 网络
the forge自带的 网络库好像基本没有
可以自己封装udp/tcp/quic，定义一种类似protobuf的二进制格式，实现帧同步状态同步
借鉴entt，raknet，boost.asio。并且内置一个内网穿透的工具方便联机

#### 1.
先把源代码拉下来（官方 GitHub，含 submodules）：其源代码以我的能力，不需要改了
git clone --recursive https://github.com/ConfettiFX/The-Forge.git
--recursive 是 Git 的一个参数，用来同时克隆仓库里的 submodule （很多第三方库，大都以二进制形式提供）
### 当然，the forge和其依赖的第三方库，如果有bug的话，还是需要跟原作者反应，在issue里提出
#### 2.
在 The Forge 根目录创建 build 文件夹：mkdir Build && cd Build
调用 CMake 生成 Ninja 构建文件：
cmake ../Examples_3/HelloWorld  \
  -G Ninja \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_BUILD_TYPE=Debug

-G Ninja → 使用 Ninja 构建（VSCode 支持 CMake + Ninja）
-DCMAKE_OSX_ARCHITECTURES=arm64 → Apple Silicon 原生
-DCMAKE_BUILD_TYPE=Debug → 开发阶段方便 debug
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON → VSCode 可以解析 IntelliSense
The Forge 默认会自动选择 Metal backend，不需要额外设置
#### 2.--编译为静态库--
cmake ../Examples_3/HelloWorld  \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DBUILD_SHARED_LIBS=OFF
解释：

-G Ninja → 用 Ninja 构建

-DCMAKE_BUILD_TYPE=Release → 发布版本

-DCMAKE_OSX_ARCHITECTURES=arm64 → Apple Silicon 原生

-DBUILD_SHARED_LIBS=OFF → 生成静态库 .a，而不是动态库 .dylib

注意：The Forge 的 CMake 会根据示例/target 自动生成静态库目标，比如 HelloWorld 会编成 libHelloWorld.a
#### 3.
选择一个最小示例，比如 Examples_3/HelloWorld：cmake --build . --target HelloWorld
或者直接用 Ninja：ninja HelloWorld
如果编译成功，你会得到可执行文件：Build/bin/HelloWorld.app/Contents/MacOS/HelloWorld
#### 3.---编译为静态库之后
ninja
Build/bin/   # 可执行文件
Build/lib/   # 静态库 .a 文件
Build/include/ # 头文件
如果你只想生成静态库，不需要编译示例，可以修改 CMake 目标，或者直接只构建 TheForgeLib（部分版本 CMake 会有 TheForge 目标
#### 4.---使用静态库
MyGame/ThirdParty/TheForge/lib/libTheForge.a
MyGame/ThirdParty/TheForge/include/...   # 必须保留
链接：
clang++ main.cpp -IThirdParty/TheForge/include \
    ThirdParty/TheForge/lib/libTheForge.a \
    -framework Metal -framework Cocoa -framework Foundation -o MyGame
shader / resource 文件：
如果你的渲染需要热加载 shader，需要保留 .metal 文件
如果 shader 预编译成 .metallib，只需拷 .metallib
