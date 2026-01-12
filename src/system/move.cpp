#include "../world/world.h"
#include "../component/component.h"

void updateMovement(World& world, float dt) {
    for (Entity e = 1; e < 1000; ++e) {
        auto* t = world.getComponent<Position>(e);
        auto* v = world.getComponent<Velocity>(e);
        if (t && v) {
            t->x += v->vx * dt;
            t->y += v->vy * dt;
        }
    }
}
void movementSystem(World& world, float dt) {
    for (auto& [e, v] : world.velocities) {
        auto* t = world.getComponent<Transform>(e);
        t->position += v.value * dt;
        v.value = {0,0,0};
    }
}
void movementSystem(World& world, float dt) {
    for (auto& [e, v] : world.velocities_) {
        auto* t = world.getComponent<Position>(e);
        if (!t) continue;

        t->position += v.value * dt; // 根据速度更新位置
        v.value = glm::vec3(0.0f);  // 清空速度
    }
}
