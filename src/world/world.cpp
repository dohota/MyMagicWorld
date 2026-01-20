#include <SDL_video.h>
#include <iostream> // std::cerr
#include "world.h"
//#include <OpenGL/glu.h> //在 macOS 上，必须额外 include GLU，半弃用的状态
void World :: setPerspective(float fov, float aspect, float zNear, float zFar) {
    float fH = std::tan(fov * 0.5f * M_PI / 180.0f) * zNear;
    float fW = fH * aspect;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
}
void World :: setupProjection(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;
    setPerspective(60.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}
World :: World() {
    // 1. 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr  << "SDL_Init error: "  << SDL_GetError() << "\n";
        return; //错误，退出程序
    }
    // 2. 创建 OpenGL 窗口（使用兼容模式）
    this->window = SDL_CreateWindow(
        "my magic world",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE//SDL_WINDOW_FULLSCREEN_DESKTOP//全屏标志
    );
    // 3. 创建 OpenGL 上下文
    this->context = SDL_GL_CreateContext(this->window);
    if (!this->context) {
        std::cerr << "CreateContext error: " << SDL_GetError() << "\n";
        return;
    }
    this->running = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);//防止鼠标没反应
    SDL_ShowCursor(SDL_DISABLE);
    this->em = new EntityManager();
    this->s = new SystemManager();
    
}
void World :: start()  {
    this->em->build("player");
    this->em->build("grass_chunk");
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
            if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                setupProjection(e.window.data1, e.window.data2);
            }
        }
        this->s->update(*(this->em), dt,this->window);
    }
}
World::~World(){
    delete this->s;
    delete this->em;
    this->s = nullptr;
    this->em = nullptr;
    SDL_GL_DeleteContext(this->context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
