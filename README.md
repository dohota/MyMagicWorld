这个分支是用java写一个mc（和原版mc最接近，甚至可以直接借鉴）
要实现lwjgl+netty+vulkan+ecs，并且以后c++的版本可以仿照我现在的版本
v 0.0.0: 初始化了lwjgl，然后用了opengl（之后要换成vulkan渲染）
v 0.0.1: lwjgl+vulkan 能正常启动空白窗口，但是test会有bug。目前的渲染还是很粗糙，且不确定跨平台是否依然表现正常
chatgpt说：警告先不用管，Vulkan/LWJGL/MoltenVK 在 macOS 上出现一些 warning 很常见
