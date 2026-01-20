#pragma once
#include <SDL.h>
#include <SDL_opengl.h> // macOS 上直接用这个
class World  {
public:
    SDL_Window* window;
    World(); //默认构造函数
    //void start();
    void update();
    ~World();

private:
    bool running;
    SDL_GLContext context;
    void setPerspective(float fov, float aspect, float zNear, float zFar);
    void setupProjection(int w, int h);

};
