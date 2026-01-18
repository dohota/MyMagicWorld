#include <iostream>
#include "../system/move.h"
#include "../system/render.h"
#include "../system/input.h"
#include "world.h"
//#include <OpenGL/glu.h> //在 macOS 上，必须额外 include GLU，半弃用的状态
Entity World::createEntity() {
    return nextEntity_++;
}
void World::destroyEntity(Entity e) {
    transforms_.erase(e);
    velocities_.erase(e);
}

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
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    // 3. 创建 OpenGL 上下文
    this->context = SDL_GL_CreateContext(this->window);
    if (!this->context) {
        std::cerr << "CreateContext error: " << SDL_GetError() << "\n";
        return;
    }
    this->running = true;
    this->start();
}
void World :: start(){
    SDL_SetRelativeMouseMode(SDL_TRUE);//防止鼠标没反应
    SDL_ShowCursor(SDL_DISABLE);
    //===== 玩家（摄像机）=====
    this->player = createEntity();
    addComponent<Position>(player, {{0, 2, 5}});
    addComponent<Velocity>(player, {{0, 0, 0}});
    addComponent<Camera>(player, {});

}
void World :: update()  {
    //===== 平原方块 =====
    for (float x = -20; x <= 10; x+=1.5) {
        for (float y = -10; y <= 3; y+=1.5){
            for (float z = -5; z <= 14; z+=1.5) {
                Entity block = createEntity();
                addComponent<Position>(block, {{x, y, z}});
            }
        }
    }
    //SDL_PumpEvents();
    Uint32 lastTime = SDL_GetTicks();
    while (this->running) {
        Uint32 current = SDL_GetTicks();
        float dt = (current - lastTime) / 1000.f;
        lastTime = current;
        // 4. 事件处理
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                this->running = false;
            if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                setupProjection(e.window.data1, e.window.data2);
            }
        }
        input_system(*this, dt);     // WASD + 鼠标
        move_system(*this, dt); // 暂时不固定 dt 为16ms
        render_system(*this);
    }
}
World::~World(){
    // 7. 清理
    SDL_GL_DeleteContext(this->context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
