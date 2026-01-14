#include <iostream>
//#include "../system/move.h"
#include "../system/render.h"
//#include "../system/input.h"
#include "world.h"
//#include <OpenGL/glu.h> //在 macOS 上，必须额外 include GLU，半弃用的状态
Entity World::createEntity() {
    return nextEntity_++;
}
template<typename T>
T* World::getComponent(Entity) {
    return nullptr;
}
template<>
Position* World::getComponent(Entity e) {
    auto it = transforms_.find(e);
    return it != transforms_.end() ? &it->second : nullptr;
}

template<>
Velocity* World::getComponent(Entity e) {
    auto it = velocities_.find(e);
    return it != velocities_.end() ? &it->second : nullptr;
}
void World::destroyEntity(Entity e) {
    transforms_.erase(e);
    velocities_.erase(e);
}

template<>
void World::addComponent(Entity e, Position c) {
    transforms_[e] = c;
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
        "Rotating Cube",
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    this->start();
}
void World :: start(){
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    int w, h;
    SDL_GetWindowSize(this->window, &w, &h);
    setupProjection(w, h);
    // ===== 玩家（摄像机）=====
    // Entity player = createEntity();
    // addComponent<Position>(player, {{0, 2, 5}});
    // addComponent<Velocity>(player, {{0, 0, 0}});
    // addComponent<Camera>(player, {});
    // ===== 平原方块 =====
    // for (int x = -20; x <= 20; x++) {
    //     for (int z = -20; z <= 20; z++) {
    //         Entity block = createEntity();
    //         addComponent<Position>(block, {{(float)x, 0.f, (float)z}});
    //         addComponent<Mesh>(block, mesh::createCubeMesh());
    //     }
    // }
}
void World :: update()  {
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
            // else if (e.type == SDL_KEYDOWN) {
            //     switch (e.key.keysym.sym) {
            //         case SDLK_ESCAPE: running=false; break;
            //         case SDLK_w: input->forward=true; break;
            //         case SDLK_s: input->backward=true; break;
            //         case SDLK_a: input->left=true; break;
            //         case SDLK_d: input->right=true; break;
            //         case SDLK_SPACE: input->up=true; break;
            //         case SDLK_LSHIFT: input->down=true; break;
            //     }
            // }
            // else if (e.type == SDL_KEYUP) {
            //     switch (e.key.keysym.sym) {
            //         case SDLK_w: input->forward=false; break;
            //         case SDLK_s: input->backward=false; break;
            //         case SDLK_a: input->left=false; break;
            //         case SDLK_d: input->right=false; break;
            //         case SDLK_SPACE: input->up=false; break;
            //         case SDLK_LSHIFT: input->down=false; break;
            //     }
            // }
            // else if (e.type == SDL_MOUSEMOTION) {
            //     input->mouseDX = e.motion.xrel;
            //     input->mouseDY = e.motion.yrel;
            // }
            if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                setupProjection(e.window.data1, e.window.data2);
            }
        }
        // // 5. OpenGL 渲染
        // glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glLoadIdentity(); //把当前矩阵重置为“单位矩阵”
        // glTranslatef(0.0f, 0.0f, -6.0f); //把整个世界 往 Z 轴负方向移动 6 个单位，便于观察
        // glRotatef(angle, 1.0f, 1.0f, 0.0f); //绕着 (x,y,z) 这个方向的轴旋转 angle 度
        // drawCube();

        // // 6. 交换缓冲
        // SDL_GL_SwapWindow(this->window);
        //input_system(*this, 0.016f);     // WASD + 鼠标
        //move_system(*this, 0.016f); // 假设固定 dt 16ms
        render_system(*this);
    }
}
World::~World(){
    // 7. 清理
    SDL_GL_DeleteContext(this->context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
