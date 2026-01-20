#pragma once
#include <SDL.h>
#include <SDL_opengl.h> // macOS 上直接用这个
#include "../entity/entity.h" 
#include "../system/system.h"
class World  {
public:
    SDL_Window* window;
    World(); //默认构造函数
    void start();
    ~World();

private:
    bool running;
    SDL_GLContext context;
    void setPerspective(float fov, float aspect, float zNear, float zFar);
    void setupProjection(int w, int h);
    SystemManager* s;
    EntityManager* em;
};
