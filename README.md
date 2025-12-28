## (branch:main)主分支
放各个版本的，有代表的，完善的，跨平台的mc引擎，专门用于制作mc类游戏（几乎所有内容都用c或c++编写）

## mce1.0:(branch:mceTextRditor)
实现文本文件编辑器（scintilla+一层简单的ui，可用imgui）

## 2.0:(branch:mceBuilder)
#### 实现自己的构建工具之前，还是用vcpkg+cmake+ninja+ccache+clang吧
实现构建工具：用c或者c++编写，仅支持构建c/c++/简单的脚本语言，可自举，可复现，性能很好增量编译快，与游戏引擎深度绑定
其内置固定版本clang，llvm，目标平台工具链。可以语法高亮，外接lsp，实时分析，自动补全，悬停提示，报错/警告。当然也可以支持其他不同类型的编译器
内置统一的包管理器：用于安装各种游戏依赖，mod，贴图等。这些资源一般放在自己的mcweb上，或者gitlfs，云盘上
甚至还可以稍微改一改编译器源代码，使其更符合项目要求，最后将编译器，构建器，包管理器一起编译成一个构建工具————类似cmake+ninja+ccache+固定版本的clang，可编译，构建，调试，自动化测试

## 3.0:(branch:mcgit)
版本控制工具（不依赖网络，可以仅本地保存）：
借鉴git：性能好，但有点难完全掌握，不适合“超大二进制文件”；
gitLFS：免费额度有限，不是真正的“版本差异存储”（每一版大文件仍是完整上传，只是不影响Git仓库体积，因为存在lfs服务器上）
perforce：收费，大规模二进制文件（tb级）性能很好，有文件锁更安全，权限与流程控制好
svn：集中式版本控制系统，所有历史和“真相”都在服务器上，简单但不灵活，对大型二进制文件支持较好（比gitLFS好）

#### mcgit：
性能很好，安全灵活，可以选择集中式版本控制/分布式版本控制，支持大型二进制文件，自带可视化工具（内嵌于游戏引擎）来比较二进制文件的差异
因为一般二进制文件每次修改几乎都是整文件变化，存储历史容易爆炸（例如 500 MB 的文件，改 50 次 → 25 GB），二进制文件不能像代码一样合并
所以我自定义二进制格式（如mc存档），实现像文本文件一样的增量存储，并且能可视化比较分析差异

## 4.0:(branch:mceIDE)
文本编辑器+构建工具+版本控制系统+ai助手（或者外接ai）一起编译，变成了一个轻量性能好，专门适应此项目的ide（类似gearry，codelite，textadept，luanti的chunk系统）
并且像clion一样自动保存，自动编译文件

# 5.0:(branch:MCEngine)
内置完整渲染管线，p2p联机系统，几乎除了底层c代码完全使用ecs架构。参考成熟引擎unity，ue；开源普通引擎o3de，torque3d
可制作/改变（内置mcIDE，有类似mcborn的可视化编辑），导出（支持交叉编译，像zig，go那样），运行游戏（内置mc启动器，像curseforge那样读取存档）
一开始实现新功能，只有引擎版本变化；等之后引擎架构，主体稳定了，新功能就只会以插件形式提供，而尽量不改变引擎版本，或者就推出几个发行版供玩家选择
修复bug一般不改变引擎版本，只是3.4.5c变为3.4.5c1（表示是该版本第一次修复bug）

## (branch:mcOS)
若自制mc引擎还是不能压榨出极致的性能，则需要自制一个专门用于运行mc的操作系统：
能适配任何硬件（可以参考linux的硬件驱动，有些闭源硬件Linux也能驱动。90%的os开发者都会在qemu虚拟机里跑内核）
可基于unix/linux内核，可以借鉴tiny core linux（仅11mb，200万多行代码，os不用完全从零开始写）：保留基本驱动+部分控制台功能（用于调试os），去除桌面gui和复杂服务
mc引擎及游戏内容最好要写进内核，以最高权限运行（尤其是chunk系统，ai系统，高性能计算系统）
甚至可以在操作系统的用户态 内置浏览器内核和wine，能够看网页和用windows上软件，甚至可以使用主流操作系统全部功能，但不能影响mc性能

## (branch:mcHardware)
可以自制/改装硬件，做一个mc游戏机，这样就能与操作系统，游戏高度贴合（借鉴各种游戏主机。最后把硬件构造设计开源）：
定制cpu/gpu架构（gpu可增加专门的chunk渲染），把它们封装在一块类似游戏主机的soc上；统一高速内存，内存布局优化（chunk，纹理，顶点按gpu缓存优化布局。预先分配对象池避免碎片化）；
存储用定制NVMe SSD；自定义i/o控制器（使ssd与cpu/gpu直接通信）；接口外设也优化；声卡优化（支持3d音效和硬件解码）；网卡正常就行；高度适配手柄

## (branch:mcweb)
个人介绍+该游戏玩法+游戏的各种衍生项目的+游戏版本控制+包管理器 可视化网站：
如果我的github仓库全都没了，那至少我的网站还在。网站不在了，那该网站的前后端代码还在，数据库还在
这个网站可以管理我各种项目的源代码，是我版本控制工具默认连接的远程仓库。也可以用做私人仓库（但是不如github上能得到更多关注，并且还要自己维护服务器）

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
