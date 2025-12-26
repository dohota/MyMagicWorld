# Magic World
#### (branch:main)主分支
放各个版本的，有代表的，完善的，跨平台的mc引擎
## 自制mc游戏引擎
专门用于制作mc类游戏，用c/c++编写，跨平台，支持交叉编译（zig，go自带）
其内置了mc启动器（类似curseforge），构建工具（类似cmake+ninja+ccache+固定版本的clang，可编译，构建，调试，自动化测试）

## mce1.0:(branch:mceTextRditor)
实现文本文件编辑器（scintilla+一层简单的ui，可用imgui）
## 2.0:(branch:mceBuilder)
### 实现自己的构建工具之前，还是用cmake+ninja+ccache+clang吧
实现构建工具：用c或者c++编写，仅支持构建c/c++/简单的脚本语言，可自举，可复现，性能很好增量编译快，与游戏引擎深度绑定
其内置固定版本clang，llvm，目标平台工具链。可以语法高亮，外接lsp，实时分析，自动补全，悬停提示，报错/警告。当然也可以支持其他不同类型的编译器
甚至还可以稍微改一改编译器源代码，使其更符合项目要求，最后将编译器与构建器一起编译成一个构建工具
## 3.0:(branch:mceIDE)
文本编辑器+构建工具+ai助手（或者外接ai）一起编译，变成了一个轻量性能好，专门适应此项目的ide（类似gearry，codelite，textadept，luanti的chunk系统）
并且像clion一样自动保存，自动编译文件
## 4.0:(branch:MCEngine)
实现游戏引擎（自带渲染管线，自带p2p联机系统，除了底层完全使用ecs架构，并且有类似mcborn的可视化编辑），可以参考unity，ue，o3de，torque3d
一开始实现新功能，只有引擎版本变化；等之后引擎架构，主体稳定了，新功能就只会以插件形式提供，而尽量不改变引擎版本，或者就推出几个发行版供玩家选择
修复bug一般不改变引擎版本，只是3.4.5c变为3.4.5c1（表示是该版本第一次修复bug）

#### (branch:mcOS)
若自制mc引擎还是不能压榨出极致的性能，则需要自制一个专门用于运行mc的操作系统：
能适配任何硬件（可以参考linux的硬件驱动，有些闭源硬件Linux也能驱动。90%的os开发者都会在qemu虚拟机里跑内核）
可基于unix/linux内核，可以借鉴tiny core linux（仅11mb，200万多行代码，os不用完全从零开始写）：保留基本驱动+部分控制台功能（用于调试os），去除桌面gui和复杂服务
mc引擎及游戏内容最好要写进内核，以最高权限运行（尤其是chunk系统，ai系统，高性能计算系统）
甚至可以在操作系统的用户态 内置浏览器内核和wine，能够看网页和用windows上软件，甚至可以使用主流操作系统全部功能，但不能影响mc性能
#### (branch:mcHardware)
可以自制/改装硬件，做一个mc游戏机，这样就能与操作系统，游戏高度贴合（借鉴各种游戏主机。最后把硬件构造设计开源）：
定制cpu/gpu架构（gpu可增加专门的chunk渲染），把它们封装在一块类似游戏主机的soc上；统一高速内存，内存布局优化（chunk，纹理，顶点按gpu缓存优化布局。预先分配对象池避免碎片化）；
存储用定制NVMe SSD；自定义i/o控制器（使ssd与cpu/gpu直接通信）；接口外设也优化；声卡优化（支持3d音效和硬件解码）；网卡正常就行；高度适配手柄
#### MyMagicWorld游戏本体：（branch：mw）
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

 ├── input_system.py
 
 ├── movement_system.py
 
 ├── collision_system.py
 
 ├── gravity_system.py
 
 ├── combat_system.py
 
 ├── pickup_system.py

render/
 ├── renderer.py
 
 ├── camera.py
 
 ├── chunk_renderer.py
 
 ├── mesh_builder.py
 
 ├── block_texture_atlas.py
 
 ├── entity_renderer.py
 
 ├── ui_renderer.py
 
 └── debug_draw.py

utils/

 ├── math.py
 
 ├── aabb.py
 
 ├── raycast.py
 
 ├── timer.py
 
 ├── config.py //各种全局配置

core/

 ├── input.py
 
 ├── keybinds.py
 
 ├── ui/
 
 │   ├── inventory_ui.py
 
 │   ├── hotbar.py
 
 │   └── crosshair.py
