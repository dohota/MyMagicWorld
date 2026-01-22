#include "../system/system.h"
//#include "../component/component.h"
//#include <iostream>
// 根据速度，和位置 决定接下来的位置
MovementSystem :: MovementSystem() {
    this->prior = 2;
    this->start();
}
void MovementSystem :: start(){
    
}
void MovementSystem :: update(EntityManager& em, float dt)  {
    // for (auto e : em.view<Position, Velocity>()) {
    //     auto* pos = em.get<Position>(e);
    //     auto* vel = em.get<Velocity>(e);
    //     if (!pos) continue;
    //     pos->position += vel->value * dt;
    //     vel->value = {0,0,0}; // 重置速度
    // }
}
MovementSystem::~MovementSystem(){
    
}
