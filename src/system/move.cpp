#include "../system/system.h"
#include "../component/component.h"
// 根据速度，和位置 决定接下来的位置
MovementSystem :: MovementSystem() {
    this->prior = 2;
    this->start();
}
void MovementSystem :: start(){
    
}
void MovementSystem :: update(World& world, float dt)  {
    for (auto& [e, vel] : world.velocities_) {
        auto* pos = world.getComponent<Position>(e);
        if (!pos) continue;

        pos->position += vel.value * dt;
        vel.value = {0,0,0}; // 重置速度
    }
}
MovementSystem::~MovementSystem(){
    
}
