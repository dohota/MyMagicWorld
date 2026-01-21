#include "../system/system.h"
#include "../entity/entity.h"
SystemManager :: SystemManager() {
    this->s1 = new InputSystem();
    this->s2 = new MovementSystem();
    this->s3 = new RenderSystem();
    this->s4 = new CollisionSystem();
    this->start();
}
void SystemManager :: start(){
    
}
void SystemManager :: update(EntityManager& em, float dt,SDL_Window* window)  {
    this->s1->update(em,dt);
    this->s2->update(em,dt);
    this->s4->update(em,dt);
    this->s3->update(em,window);
}
SystemManager::~SystemManager(){
    delete this->s1;
    delete this->s2;
    delete this->s3;
    delete this->s4;
    this->s1 = nullptr;
    this->s2 = nullptr;
    this->s3 = nullptr;
    this->s4 = nullptr;
}
//System s;------栈
// 	System* s = new System();-------堆
