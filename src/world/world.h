#pragma once
#include <SDL.h>
// 或者用 include guard

//namespace math {

class World  {
public:
    World(); //默认构造函数
    void start();
    void update();
    ~World(); 

private:
    bool running;
    SDL_Window* window;
    SDL_GLContext context;
    void setPerspective(float fov, float aspect, float zNear, float zFar);
    void setupProjection(int w, int h);
    void drawCube();
    
};

//} 
