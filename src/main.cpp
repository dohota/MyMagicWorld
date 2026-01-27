// #include "world/world.h"
// int main(int argc, char** argv) {
// #if defined(__APPLE__)
//     @autoreleasepool {
//         World w;
//         w.start();
//     }
// #endif
//     return 0;
// }
#include <SDL.h>
#include <SDL_metal.h>
#include <SDL_video.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "bgfx test",
        100, 100, 800, 600,
        SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_METAL
    );

    void* metalView = SDL_Metal_CreateView(window);

    bgfx::PlatformData pd{};
    pd.nwh = metalView;
    bgfx::setPlatformData(pd);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    // 🔥🔥🔥 关键中的关键
    bgfx::renderFrame();

    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setDebug(BGFX_DEBUG_STATS);

    // bgfx::RendererType::Enum renderers[BGFX_RENDERER_TYPE_COUNT];
    // uint8_t count = bgfx::getSupportedRenderers(renderers);

    // printf("Supported renderers:\n");
    // for (uint8_t i = 0; i < count; ++i) {
    //     printf("  %d\n", renderers[i]);
    // }

    bgfx::Init init{};
    init.type = bgfx::RendererType::Metal;
    init.resolution.width  = w * 2;
    init.resolution.height = h * 2;
    init.resolution.reset  = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
        printf("bgfx init failed\n");
        return -1;
    }

    bgfx::setViewClear(
        0,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x303030ff, 1.0f, 0
    );

    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                return 0;
        }

        bgfx::touch(0);
        bgfx::frame();
    }
}
