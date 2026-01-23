#include "../system/system.h"
#include "../entity/entity.h"
SystemManager :: SystemManager(EntityManager& em, EventBus& ev) {
    this->s1 = new InputSystem();
    this->s2 = new RaycastSystem();
    this->s3 = new RenderSystem();
    this->s4 = new CollisionSystem();
    this->s5 = new EntitySystem(em, ev);
    this->start();
}
void SystemManager :: start(){
    
}
void SystemManager :: update(EntityManager& em, EventBus& ev, float dt,SDL_Window* window)  {
    this->s1->update(em,ev,dt);
    this->s4->update(em,dt);
    this->s2->update(em,ev,dt);
    this->s5->update(em,ev,dt);
    this->s3->update(em,ev,window);
}
SystemManager::~SystemManager(){
    delete this->s1;
    delete this->s2;
    delete this->s3;
    delete this->s4;
    delete this->s5;
    this->s1 = nullptr;
    this->s2 = nullptr;
    this->s3 = nullptr;
    this->s4 = nullptr;
    this->s5 = nullptr;
}
//System s;------栈
// 	System* s = new System();-------堆
