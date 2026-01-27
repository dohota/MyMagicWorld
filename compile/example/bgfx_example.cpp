#include <SDL_video.h>
#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>

//#include <SDL3/SDL.h>
#include <SDL.h>
#include <SDL2/SDL_syswm.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/file.h>
#include <bimg/bimg.h>
#include <bx/allocator.h>
#include <bx/debug.h>

#include <bx/filepath.h>
#include <bgfx/bgfx.h>
#include <bx/allocator.h>//BX_NEW();

#include <bgfx/bgfx.h>               // bgfx渲染API核心接口
#include <bx/endian.h>            // bx字节序处理（纹理数据可能涉及跨平台字节序）
#include <bx/math.h>              // bx数学工具（纹理尺寸计算等可能用到）
#include <bx/readerwriter.h>      // bx文件读写接口（FileReaderI等）
#include <bx/string.h>            // bx字符串处理（文件路径操作等）
#include <bx/debug.h>             // bx调试工具（打印错误信息等）
#include <bimg/decode.h>             // bimg图像解码接口（解析纹理数据）
#include <bx/allocator.h>         // bx内存分配器（管理纹理数据内存）

/**
 * 纹理图像数据释放回调函数
 * 当bgfx不再需要纹理数据时，会调用此函数释放bimg解析后的图像容器
 * @param _ptr  未使用（bgfx传递的内存指针，此处无需处理）
 * @param _userData 指向bimg::ImageContainer的指针（需释放的图像容器）
 */
static void imageReleaseCb(void* _ptr, void* _userData)
{
    BX_UNUSED(_ptr);  // 标记参数未使用，避免编译警告
    // 将用户数据转换为图像容器指针
    bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
    // 释放bimg图像容器（内部会释放其管理的像素数据）
    bimg::imageFree(imageContainer);
}

/**
 * 从文件加载原始数据到内存
 * @param _reader   bx文件读取接口（负责实际读写操作）
 * @param _allocator 内存分配器（用于分配存储文件数据的内存）
 * @param _filePath 要加载的文件路径
 * @param _size     输出参数，返回加载的数据大小（字节）
 * @return 加载的原始数据指针（需通过unload释放），失败返回NULL
 */
void* load(bx::FileReaderI* _reader, bx::AllocatorI* _allocator, const bx::FilePath& _filePath, uint32_t* _size)
{
    // 尝试打开文件
    if (bx::open(_reader, _filePath))
    {
        // 获取文件大小
        uint32_t size = (uint32_t)bx::getSize(_reader);
        // 分配内存存储文件数据
        void* data = bx::alloc(_allocator, size);
        // 读取文件数据到内存（使用ErrorAssert确保读取成功，失败会触发断言）
        bx::read(_reader, data, size, bx::ErrorAssert{});
        // 关闭文件（释放文件句柄）
        bx::close(_reader);

        // 输出数据大小（如果需要）
        if (NULL != _size)
        {
            *_size = size;
        }
        return data;  // 返回加载的数据指针
    }
    else
    {
        // 打开文件失败，打印错误信息
        bx::debugPrintf("Failed to open: %s.", _filePath.getCPtr());
    }

    // 加载失败，重置输出大小
    if (NULL != _size)
    {
        *_size = 0;
    }
    return NULL;
}

/**
 * 释放由load函数分配的内存
 * @param _allocator 用于分配内存的分配器（需与load时一致）
 * @param _ptr       要释放的内存指针
 */
void unload(bx::AllocatorI* _allocator, void* _ptr)
{
    bx::free(_allocator, _ptr);  // 调用bx的内存释放函数
}


/**
 * 加载纹理文件并创建bgfx纹理句柄（核心实现）
 * 支持解析多种格式（如PNG、KTX v1等），并转换为bgfx可直接使用的纹理
 * @param _reader     bx文件读取接口
 * @param _filePath   纹理文件路径
 * @param _allocator  内存分配器
 * @param _flags      纹理创建标志（如生成Mipmap、sRGB等，见bgfx文档）
 * @param _skip       跳过的Mipmap层级（当前未使用，预留扩展）
 * @param _info       输出参数，返回纹理信息（宽高、格式等，可选）
 * @param _orientation 输出参数，返回纹理方向（如是否翻转，可选）
 * @return bgfx纹理句柄（无效时为BGFX_INVALID_HANDLE）
 */
bgfx::TextureHandle loadTexture(
    bx::FileReaderI* _reader,
    const bx::FilePath& _filePath, 
    bx::AllocatorI* _allocator,
    uint64_t _flags, 
    uint8_t _skip, 
    bgfx::TextureInfo* _info, 
    bimg::Orientation::Enum* _orientation)
{
    BX_UNUSED(_skip);  // 标记参数未使用（预留，暂未实现Mipmap跳过逻辑）
    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;  // 初始化纹理句柄为无效

    // 加载文件原始数据（二进制）
    uint32_t size;
    void* data = load(_reader, _allocator, _filePath, &size);
    if (NULL != data)  // 数据加载成功
    {
        // 用bimg解析原始数据，生成图像容器（包含像素数据、格式、宽高等信息）
        // 支持的格式由bimg决定（如PNG、KTX v1、DDS等）
        bimg::ImageContainer* imageContainer = bimg::imageParse(_allocator, data, size);

        if (NULL != imageContainer)  // 图像解析成功
        {
            // 如果需要，输出纹理方向信息（如图像是否需要翻转）
            if (NULL != _orientation)
            {
                *_orientation = imageContainer->m_orientation;
            }

            // 创建bgfx内存引用：关联bimg解析后的像素数据，
            // 并指定释放回调（当bgfx不再使用时，自动释放imageContainer）
            const bgfx::Memory* mem = bgfx::makeRef(
                  imageContainer->m_data          // 像素数据指针
                , imageContainer->m_size          // 数据大小
                , imageReleaseCb                  // 释放回调函数
                , imageContainer                  // 传递给回调的用户数据（图像容器）
                );

            // 释放原始文件数据（已被bimg解析，不再需要）
            unload(_allocator, data);

            // 如果需要，计算并输出纹理信息（宽高、Mipmap数量、格式等）
            if (NULL != _info)
            {
                bgfx::calcTextureSize(
                    *_info
                    , uint16_t(imageContainer->m_width)    // 宽度
                    , uint16_t(imageContainer->m_height)   // 高度
                    , uint16_t(imageContainer->m_depth)    // 深度（3D纹理用）
                    , imageContainer->m_cubeMap            // 是否为立方体贴图
                    , 1 < imageContainer->m_numMips        // 是否有Mipmap（层级>1）
                    , imageContainer->m_numLayers          // 纹理数组层数
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)  // 纹理格式
                );
            }

            // 根据图像类型创建对应类型的bgfx纹理
            if (imageContainer->m_cubeMap)  // 立方体贴图
            {
                handle = bgfx::createTextureCube(
                      uint16_t(imageContainer->m_width)
                    , 1 < imageContainer->m_numMips
                    , imageContainer->m_numLayers
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , _flags
                    , mem
                    );
            }
            else if (1 < imageContainer->m_depth)  // 3D纹理（深度>1）
            {
                handle = bgfx::createTexture3D(
                      uint16_t(imageContainer->m_width)
                    , uint16_t(imageContainer->m_height)
                    , uint16_t(imageContainer->m_depth)
                    , 1 < imageContainer->m_numMips
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , _flags
                    , mem
                    );
            }
            else  // 2D纹理（默认）
            {
                // 先检查纹理格式和标志是否合法
                if (bgfx::isTextureValid(0, false, imageContainer->m_numLayers, bgfx::TextureFormat::Enum(imageContainer->m_format), _flags))
                {
                    handle = bgfx::createTexture2D(
                          uint16_t(imageContainer->m_width)
                        , uint16_t(imageContainer->m_height)
                        , 1 < imageContainer->m_numMips
                        , imageContainer->m_numLayers
                        , bgfx::TextureFormat::Enum(imageContainer->m_format)
                        , _flags
                        , mem
                        );
                }
            }

            // 如果纹理创建成功，给纹理命名（方便调试工具识别）
            if (bgfx::isValid(handle))
            {
                const bx::StringView name(_filePath);
                bgfx::setName(handle, name.getPtr(), name.getLength());
            }
        }
    }

    return handle;  // 返回创建的纹理句柄（可能无效）
}

/**
 * 加载纹理的重载函数（调整参数顺序，方便调用）
 * 实际调用上面的核心实现，仅参数顺序不同
 */
bgfx::TextureHandle loadTexture(
    const bx::FilePath& _filePath,
    bx::FileReaderI* _reader, 
    bx::AllocatorI* _allocator,
    bgfx::TextureInfo* _info, 
    uint64_t _flags, 
    uint8_t _skip, 
    bimg::Orientation::Enum* _orientation)
{
    // 转发到核心实现函数（仅参数顺序调整）
    return loadTexture(_reader, _filePath, _allocator, _flags, _skip, _info, _orientation);
}

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>  // Windows.h 头文件，获取 HDC 需要
#endif

// 跨平台：获取 SDL3 窗口句柄，填充 bgfx PlatformData
// void setupBgfxPlatformData(SDL_Window* window, bgfx::PlatformData& pd) {
//     // 初始化 PlatformData 为 0（关键：避免随机无效值）
//     memset(&pd, 0, sizeof(pd));

// #if defined(_WIN32) || defined(WIN32)  // Windows 平台（Windows 11 适用）
//     SDL_PropertiesID windowProps = SDL_GetWindowProperties(window);
//     // 核心：Windows 平台获取 HWND（窗口句柄），赋值给 pd.nwh
//     pd.nwh = SDL_GetPointerProperty(
//         windowProps, 
//         SDL_PROP_WINDOW_WIN32_HWND_POINTER,  // SDL3 Windows 专属句柄属性
//         nullptr                               // 找不到时返回默认值 nullptr
//     );
//     // 关键：设置窗口句柄类型（告诉 bgfx 这是 Windows 的 HWND）
//     pd.type = bgfx::NativeWindowHandleType::Default;
//     // 其他成员（ndt/context/backBuffer 等）：bgfx 自动创建，无需手动设置

// #elif defined(__linux__)  // Linux X11 平台（保留你的原逻辑，修正句柄类型）
//     SDL_PropertiesID windowProps = SDL_GetWindowProperties(window);
//     pd.ndt = SDL_GetPointerProperty(windowProps, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
//     uint64_t x11WindowNum = SDL_GetNumberProperty(windowProps, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
//     pd.nwh = reinterpret_cast<void*>(static_cast<uintptr_t>(x11WindowNum));
//     pd.type = bgfx::NativeWindowHandleType::Default;  // 明确 X11 句柄类型

// #elif defined(__APPLE__)  // macOS 平台（可选，跨平台兼容用）
//     SDL_PropertiesID windowProps = SDL_GetWindowProperties(window);
//     pd.ndt = SDL_GetPointerProperty(windowProps, SDL_PROP_WINDOW_COCOA_NS_WINDOW_POINTER, nullptr);
//     pd.nwh = SDL_GetPointerProperty(windowProps, SDL_PROP_WINDOW_COCOA_NS_VIEW_POINTER, nullptr);
//     pd.type = bgfx::NativeWindowHandleType::Default;

// #endif
// }
#include <SDL_syswm.h>

void setupBgfxPlatformData(SDL_Window* window, bgfx::PlatformData& pd)
{
    memset(&pd, 0, sizeof(pd));

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);

    if (!SDL_GetWindowWMInfo(window, &wmi))
    {
        SDL_Log("SDL_GetWindowWMInfo failed: %s", SDL_GetError());
        return;
    }

#if defined(_WIN32)
    pd.nwh = wmi.info.win.window;

#elif defined(__APPLE__)
    // macOS + Metal（最稳）
    pd.nwh = SDL_Metal_CreateView(window);
    pd.ndt = nullptr;

#elif defined(__linux__)
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#endif
}

// 顶点结构体（位置 + 颜色 + 纹理坐标）
struct PosColorTexVertex
{
    float x, y, z;      // 位置
    uint32_t abgr;      // 颜色（ABGR格式）
    float u, v;         // 纹理坐标（新增）

    // 获取顶点布局（包含纹理坐标）
    static bgfx::VertexLayout getLayout()
    {
        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)       // 位置
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)  // 颜色（归一化）
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)     // 纹理坐标（新增）
            .end();
        return layout;
    }
};

// 正方形顶点数据（4个顶点，2个三角形组成正方形）
static PosColorTexVertex squareVertices[] = {
    // 左下角
    { -0.5f, -0.5f, 0.0f, 0xffffffff, 0.0f, 1.0f },
    // 右下角
    {  0.5f, -0.5f, 0.0f, 0xffffffff, 1.0f, 1.0f },
    // 右上角
    {  0.5f,  0.5f, 0.0f, 0xffffffff, 1.0f, 0.0f },
    // 左上角
    { -0.5f,  0.5f, 0.0f, 0xffffffff, 0.0f, 0.0f },
};

// 索引数据（2个三角形，共6个索引）
static const uint16_t squareIndices[] = {
    0, 1, 2,  // 第一个三角形
    0, 2, 3   // 第二个三角形
};

// 从文件加载着色器
static bgfx::ShaderHandle loadShader(const char* filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "无法打开着色器文件: " << filePath << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "读取着色器文件失败: " << filePath << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    const bgfx::Memory* mem = bgfx::copy(buffer.data(), buffer.size());
    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, filePath);
    
    std::cout << "加载着色器: " << filePath << " 大小: " << size << " 字节" << std::endl;
    return handle;
}

int main(int argc, char* args[]) {
    const int WIDTH = 800;
    const int HEIGHT = 600;
    SDL_Window* window = nullptr;
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL初始化失败: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow(
        "my magic world",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        /* SDL_WINDOW_METAL |*/ SDL_WINDOW_FULLSCREEN_DESKTOP // 全屏标志
    ); 
    if (!window) {
        printf("窗口创建失败: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    bgfx::PlatformData pd;
    setupBgfxPlatformData(window, pd);
    bgfx::setPlatformData(pd);

    // SDL_PropertiesID windowProps = SDL_GetWindowProperties(window);
    // bgfx::PlatformData pd = {};
    // setupBgfxPlatformData(window, pd);
    // bgfx::setPlatformData(pd);
    
    // 初始化bgfx
    bgfx::Init init;
    init.platformData = pd;
    init.resolution.width = WIDTH;
    init.resolution.height = HEIGHT;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.type = bgfx::RendererType::Vulkan;    // 直接指定 Vulkan 后端！
    
    if (!bgfx::init(init)) {
        fprintf(stderr, "BGFX初始化失败\n");
        return 1;
    }

    // 全局分配器和文件读写器
    //zhan
    //bx::DefaultAllocator defaultAlloc;
    //bx::AllocatorI* g_allocator = &defaultAlloc;
    //dui
    bx::AllocatorI* g_allocator = new bx::DefaultAllocator();
    static bx::FileReaderI* fileReader = BX_NEW(g_allocator, bx::FileReader);

    // 加载纹理（使用带纹理信息的重载）
    bgfx::TextureInfo texInfo;
    //----bgfx::TextureHandle texture = loadTexture("../pygame.png",fileReader,g_allocator,&texInfo);

    // 检查纹理加载状态
    // ------if (!bgfx::isValid(texture)) {
    //     bx::debugPrintf("纹理加载失败！路径: resources/pygame.png\n");
    //     bgfx::shutdown();
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     return 1;
    // } else {
    bx::debugPrintf("纹理加载成功：\n");
    bx::debugPrintf("宽度：%d, 高度：%d\n", texInfo.width, texInfo.height);
    bx::debugPrintf("格式：%d (bgfx纹理格式)\n", texInfo.format);

    // 打印使用的渲染器
    bgfx::RendererType::Enum renderer = bgfx::getRendererType();
    std::cout << "使用的渲染器: ";
    switch (renderer) {
        case bgfx::RendererType::OpenGL:    std::cout << "OpenGL"; break;
        case bgfx::RendererType::Vulkan:    std::cout << "Vulkan"; break;
        case bgfx::RendererType::Metal:     std::cout << "Metal"; break;
        case bgfx::RendererType::Direct3D11: std::cout << "Direct3D 11"; break;
        case bgfx::RendererType::Direct3D12: std::cout << "Direct3D 12"; break;
        default: std::cout << "未知";
    }
    std::cout << std::endl;

    // 设置视图（清屏颜色：浅灰色）
    bgfx::setViewRect(0, 0, 0, WIDTH, HEIGHT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xf0f0f0ff, 1.0f, 0);

    // 创建顶点缓冲区（正方形顶点）
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(squareVertices, sizeof(squareVertices)),
        PosColorTexVertex::getLayout()
    );

    // 创建索引缓冲区（绘制正方形需要索引）
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(squareIndices, sizeof(squareIndices))
    );

    if (!bgfx::isValid(vbh) || !bgfx::isValid(ibh)) {
        fprintf(stderr, "顶点/索引缓冲区创建失败！\n");
        bgfx::shutdown();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 加载支持纹理采样的着色器（注意：需要对应修改着色器代码）
    // 顶点着色器：传递纹理坐标到片段着色器
    // 片段着色器：使用纹理坐标采样纹理
    bgfx::ShaderHandle vsh = loadShader("shaders/vulkan/vs_rect.bin");    // 带纹理坐标的顶点着色器
    bgfx::ShaderHandle fsh = loadShader("shaders/vulkan/fs_rect.bin");    // 带纹理采样的片段着色器
    
    if (!bgfx::isValid(vsh) || !bgfx::isValid(fsh)) {
        fprintf(stderr, "着色器加载失败！\n");
        bgfx::shutdown();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    bgfx::UniformHandle texUniform = bgfx::createUniform("u_tex0", bgfx::UniformType::Sampler);
    if (!bgfx::isValid(texUniform)) {
        fprintf(stderr, "纹理采样器Uniform创建失败！\n");
        bgfx::destroy(program);
        bgfx::shutdown();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 主循环
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        // 处理事件
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_WINDOWEVENT_RESIZED) {
                int newW = event.window.data1;
                int newH = event.window.data2;
                bgfx::reset(newW, newH, BGFX_RESET_VSYNC);
                bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(newW), static_cast<uint16_t>(newH));
            }
        }

        // 开始新帧
        bgfx::touch(0);

        // 绑定顶点缓冲区和索引缓冲区
        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        // 绑定纹理到纹理单元0（与着色器中的采样器对应）
        //-----bgfx::setTexture(0, texUniform, texture);
        //bgfx::setTexture(0, bgfx::UniformHandle(0), texture);  // 假设采样器在着色器中是location 0

        // 设置渲染状态（启用深度测试和混合）
        bgfx::setState(
            BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
            BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA
        );

        // 提交绘制命令（绘制索引缓冲区中的三角形）
        bgfx::submit(0, program);

        // 结束帧
        bgfx::frame();

        SDL_Delay(16.0f);
    }

    // 清理资源
    bgfx::destroy(vbh);
    bgfx::destroy(ibh);
    bgfx::destroy(program);
    //----bgfx::destroy(texture);
    //bgfx::destroy(g_allocator);
    bgfx::destroy(texUniform);
    //bgfx::destroy(fileReader);
    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

// clang++ main.cpp -o myprogram

// clang++ → 使用 C++ 编译器
// main.cpp → 你要编译的源文件
// -o myprogram → 输出可执行文件名字为 myprogram

// 运行：./myprogram