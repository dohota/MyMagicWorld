#include <iostream>
#include <cmath>
#include "world.h"
//#include <OpenGL/glu.h> //在 macOS 上，必须额外 include GLU，半弃用的状态
Entity World::createEntity() {
    return nextEntity_++;
}

void World::destroyEntity(Entity e) {
    transforms_.erase(e);
    velocities_.erase(e);
}

template<>
void World::addComponent(Entity e, Position c) {
    transforms_[e] = c;
}

template<>
void World::addComponent(Entity e, Velocity c) {
    velocities_[e] = c;
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

void World :: drawCube() {
    glBegin(GL_QUADS);

    // Front
    glColor3f(1, 0, 0);
    glVertex3f(-1, -1,  1);
    glVertex3f( 1, -1,  1);
    glVertex3f( 1,  1,  1);
    glVertex3f(-1,  1,  1);

    // Back
    glColor3f(0, 1, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1,  1, -1);
    glVertex3f( 1,  1, -1);
    glVertex3f( 1, -1, -1);

    // Left
    glColor3f(0, 0, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1,  1);
    glVertex3f(-1,  1,  1);
    glVertex3f(-1,  1, -1);

    // Right
    glColor3f(1, 1, 0);
    glVertex3f( 1, -1, -1);
    glVertex3f( 1,  1, -1);
    glVertex3f( 1,  1,  1);
    glVertex3f( 1, -1,  1);

    // Top
    glColor3f(0, 1, 1);
    glVertex3f(-1,  1, -1);
    glVertex3f(-1,  1,  1);
    glVertex3f( 1,  1,  1);
    glVertex3f( 1,  1, -1);

    // Bottom
    glColor3f(1, 0, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f( 1, -1, -1);
    glVertex3f( 1, -1,  1);
    glVertex3f(-1, -1,  1);

    glEnd();
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
    start();
}
void World :: start(){
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    int w, h;
    SDL_GetWindowSize(this->window, &w, &h);
    setupProjection(w, h);
}
void World :: update()  {
    float angle = 0.0f;
    while (this->running) {
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
        angle += 0.03f; // 调整旋转速度
        // 5. OpenGL 渲染
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity(); //把当前矩阵重置为“单位矩阵”
        glTranslatef(0.0f, 0.0f, -6.0f); //把整个世界 往 Z 轴负方向移动 6 个单位，便于观察
        glRotatef(angle, 1.0f, 1.0f, 0.0f); //绕着 (x,y,z) 这个方向的轴旋转 angle 度
        drawCube();

        // 6. 交换缓冲
        SDL_GL_SwapWindow(this->window);
    }   
}
World::~World(){
    // 7. 清理
    SDL_GL_DeleteContext(this->context);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}
