#include "world.h"
World :: World() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return; // 初始化 SDL
    
#if defined(__APPLE__)
    this->window = SDL_CreateWindow(
        "my magic world",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_METAL 
        //SDL_WINDOW_FULLSCREEN_DESKTOP // 全屏标志
    ); 
    // 在apple上要用 SDL_WINDOW_METAL
#else
    this->window = SDL_CreateWindow(
        "my magic world",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
    ); 
#endif

    if (!this->window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return;
    }else {
        this->running = true;
        SDL_SetRelativeMouseMode(SDL_TRUE);//防止鼠标没反应
        SDL_ShowCursor(SDL_DISABLE);
        this->em = new EntityManager();
        this->s = new SystemManager(this->window);
        this->ev = new EventBus();
        this->cv = new CommandBuffer();
    }
}

void World :: start()  {
    this->em->build("player",{0,0,0});
    this->em->build("grass_chunk",{0,0,0});
    //SDL_PumpEvents();
    Uint32 lastTime = SDL_GetTicks();
    while (this->running) {
        Uint32 current = SDL_GetTicks();
        float dt = (current - lastTime) / 1000.f;
        lastTime = current;
        
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                this->running = false;
            // if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            //     setupProjection(e.window.data1, e.window.data2);
            // }
        }
        this->s->update(*(this->em), *(this->cv), *(this->ev), dt);
    }
}

World::~World(){
    bgfx::shutdown();
    delete this->s;
    delete this->em;
    delete this->ev;
    delete this->cv;
    this->s = nullptr;
    this->em = nullptr;
    this->ev = nullptr;
    this->cv = nullptr;
    SDL_DestroyWindow(window);
    SDL_Quit();
}
