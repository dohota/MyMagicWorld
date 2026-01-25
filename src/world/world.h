#pragma once
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
    SystemManager* s;
    EntityManager* em;
    EventBus* ev;
    CommandBuffer* cv;
};
