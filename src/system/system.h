#pragma once
#include <SDL.h>
#include "../entity/entity.h"
class InputSystem  {
public:
    int prior = 1; //是第几个运行的系统
    InputSystem(); //默认构造函数
    void start();
    void update(EntityManager& em, float dt);
    ~InputSystem();
};

class MovementSystem  {
public:
    int prior = 2;
    MovementSystem(); //默认构造函数
    void start();
    void update(EntityManager& em, float dt);
    ~MovementSystem();
};

class RenderSystem  {
public:
    int prior = 3;
    RenderSystem(); //默认构造函数
    void start();
    void update(EntityManager& em, SDL_Window* window);
    ~RenderSystem();
private:
    void drawCube(float x, float y, float z, float s = 1.5f);
    void gluPerspective(float fov, float aspect, float zNear, float zFar);
};

class SystemManager  {
public:
    SystemManager(); //默认构造函数
    void start();
    void update(EntityManager& em, float dt,SDL_Window* window);
    ~SystemManager();

private:
    InputSystem*    s1 = nullptr;
    MovementSystem* s2 = nullptr;
    RenderSystem*   s3 = nullptr;
};
