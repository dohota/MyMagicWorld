## Code：
仓库真正存放代码和文件的地方
#### Issues：
项目的“问题追踪系统”，是“项目讨论区 + 任务列表”，包括 报告bug，记录待办事项
#### Pull requests：
协作开发的核心机制————请求合并，合并到主分支
#### discussions：
讨论该项目的使用，体验，提出未来的建议。issues主要用来报告bug，和一些重要的/有潜在风险的功能
#### Actions：
自动化流水线系统（CI/CD）————让“机器帮你干重复的活”
可以自动编译代码，自动跑测试，自动打包，自动发布版本，自动检查代码格式
#### Projects：
项目看板 / 任务管理工具。即轻量Jira
适合规划功能，管理 Issues / PR，小团队项目管理
和 Issues 的关系：Project 是“宏观管理”，Issues 是“具体事项”
## Wiki：
README只是入口介绍
wiki是项目的详细文档系统，知识库
#### 我基本上把重要的介绍内容，各个版本的设想全都写入wiki
用于说明 怎么使用该项目？架构设计是什么？api文档是哪些？开发规范是什么？
#### Security：
安全相关功能集合，避免“公开泄露漏洞”
包含漏洞报告（私下报告安全问题），依赖安全扫描，安全公告（Advisories）
第三方库有漏洞 → GitHub 提醒你，用户发现安全问题 → 私下通知你
#### Insights：
仓库的“数据分析中心”，看项目健康度和活跃度
#### Settings：
仓库的控制中心（仅管理员可见）：管理权限，开关 Issues / Wiki / Projects，设置默认分支，设置保护规则（禁止直接 push main），配置 Actions 权限，删除仓库（慎重）

#### 1
有了vcpkg.json：vcpkg 自动进入 manifest 模式, VS Code + CMake Tools 会自动配合：
✅ 不用手动执行 vcpkg install xxx
✅ 只改 vcpkg.json
✅ CMake / VS Code 会自动下载、自动删除

### 2
vcpkg 所管理的库叫 ports
每个库都有一个目录和配置文件（portfile.cmake），指定：
源码 URL（通常是 GitHub、官方 release 压缩包）
依赖的其他库
构建选项（CMake、编译宏、patch 等）

### 3
该项目配置：目前用的是m4系列的macos，ide用vscode（装了clangd和cmake的插件）
brew下载了cmake，ninja，vcpkg，然后项目好像就能自动识别了
点击左下角的启动按钮，就能自动启动项目了


## 项目构思：
先用sdl2+opengl，然后可以试试加上openal和raknet/boost.asio,ui就使用imgui，物理引擎也可以抄开源的
再试试换用sdl3/glfw是什么效果（可以仿照minetest的代码）
接下来再试试用bgfx/filament换掉opengl，然后用the forge来写（性能很好），最后全都自己手写，可以参考sokol库
构建方面暂定：cmake+ninja+vcpkg，等项目超过十万行再考虑别的

底层和操作系统/硬件打交道的代码用c语言，上层用c++ ecs架构，其中chunk会特殊一点，一堆方块视为一个entity
## v1.x:
src/main.cpp：用于创建窗口，开始游戏（world）

    /world：
    world.cpp：管理system，entity，component，管理chunk
    chunk.c：管理chunk（一大组方块），block（单个方块）

    /system：
    input.c：输入系统
    render.c：渲染系统 （含ui绘制）
    system.cpp: 移动系统，战斗系统
    camera.c：
    
    /game_object：
    entity.cpp：允许增加删除组件，查找组件。用于创造实体（如玩家，掉落物等小东西，不适合创造百万个方块）
    component.c：实体的组件(一般都是结构体)

    /util：
        ├── math.c 有关数学类的
        ├── aabb.c
        ├── raycast.c
        ├── timer.c
        ├── config.c 各种全局配置

    /core：
    shader.xx：OpenGL着色器等配置
    # 有关窗口，输入，声音，渲染的底层库，之后这里可以放sokol文件

### 之后项目再大，可以写成这样：
<!-- world/
 ├── world.py        
 ├── ecs.py          # entity / component / system 管理
 ├── chunk_manager.py
 ├── event_bus.py    #事件系统，job system什么的 --> 事件总线让系统间只通过“事件”通信，降低依赖
一个chunk是一个实体，包含很多block。特殊block可被当作实体
 world/
 ├── world.py  # 生命周期 & 总调度
 ├── chunk.py
 ├── chunk_manager.py
 ├── block.py
 ├── block_registry.py
 ├── world_gen.py
 ├── save_load.py

ecs/
 ├── entity.py
 ├── component.py
 ├── system.py
 ├── ecs_world.py

gameobject/
 ├── player.py
 ├── item_entity.py
 ├── components/
 │   ├── transform.py
 │   ├── velocity.py
 │   ├── collider.py
 │   ├── health.py
 │   ├── inventory.py
 │   └── camera.py

systems/
    render_system/
        ├── renderer.py
        ├── camera.py
        ├── chunk_renderer.py
        ├── mesh_builder.py
        ├── block_texture_atlas.py
        ├── entity_renderer.py
        ├── ui_renderer.py
        └── debug_draw.py
    ├── input_system.py
    ├── movement_system.py
    ├── collision_system.py
    ├── gravity_system.py
    ├── combat_system.py
    ├── pickup_system.py

core/
 ├── input.py
 ├── keybinds.py
 ├── ui/
 │   ├── inventory_ui.py
 │   ├── hotbar.py
 │   └── crosshair.py

# v1.0.0-v1.0.1
