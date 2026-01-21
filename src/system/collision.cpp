#include "../system/system.h"

CollisionSystem :: CollisionSystem() {
    this->prior = 2;
    this->start();
}
void CollisionSystem :: start(){
    
}
// 所有碰撞体 两两检测
// 判断相交的同时，处理位移

// 按照速度 / 位移裁剪
// 按轴分离
void CollisionSystem :: update(EntityManager& em, float dt)  {
    for (auto e : em.view<Position,Collider>()) { //查询哪些实体是碰撞体
        auto* pos = em.get<Position>(e);
        auto* vel  = em.get<Velocity>(e);
        auto* s  = em.get<Size>(e);
        auto* coll = em.get<Collider>(e);
        // coll->box.min.x = pos->position.x;
        // coll->box.min.y = pos->position.y;
        // coll->box.min.z = pos->position.z;
        // coll->box.max.x = pos->position.x + s->value.x;
        // coll->box.max.y = pos->position.y + s->value.y;
        // coll->box.max.z = pos->position.z + s->value.z;
        if(!vel) continue; //没有速度的entity采用另一种方式检测碰撞
        Vec3 move = vel->value * dt;
        // X → Y → Z（MC 核心）
        move.x = resolveAxis(em, e, coll->box, move.x, Axis::X);
        coll->box.offset(move.x, 0, 0);

        move.y = resolveAxis(em, e, coll->box, move.y, Axis::Y);
        coll->box.offset(0, move.y, 0);

        move.z = resolveAxis(em, e, coll->box, move.z, Axis::Z);
        coll->box.offset(0, 0, move.z);

        pos->position = coll->box.getCenter();
    }
}
//按轴裁剪位移
float CollisionSystem::resolveAxis(EntityManager& em,Entity self,const AABB& box,float d, Axis axis) {
    if (d == 0.0f) return 0.0f;
    float result = d;
    for (auto other : em.view<Collider>()) {
        if (other == self) continue;
        const AABB& b = em.get<Collider>(other)->box;
        result = clip(box, b, result, axis);
    }
    return result;
}
// 真正的 MC 碰撞裁剪（核心中的核心）
float CollisionSystem::clip(const AABB& a,const AABB& b,float d,Axis axis) {
    if (!overlapsOnOtherAxes(a, b, axis))
        return d;

    if (d > 0) {
        float max = getMin(b, axis) - getMax(a, axis);
        if (max < d) d = max;
    } else {
        float min = getMax(b, axis) - getMin(a, axis);
        if (min > d) d = min;
    }
    return d;
}
// 辅助函数（必须有）
bool CollisionSystem::overlapsOnOtherAxes(const AABB& a,const AABB& b,Axis axis) {
    switch (axis) {
        case Axis::X:
            return a.max.y > b.min.y && a.min.y < b.max.y &&
                   a.max.z > b.min.z && a.min.z < b.max.z;
        case Axis::Y:
            return a.max.x > b.min.x && a.min.x < b.max.x &&
                   a.max.z > b.min.z && a.min.z < b.max.z;
        case Axis::Z:
            return a.max.x > b.min.x && a.min.x < b.max.x &&
                   a.max.y > b.min.y && a.min.y < b.max.y;
    }
    return false;
}
float CollisionSystem::getMin(const AABB& b, Axis a) {
    return a == Axis::X ? b.min.x :
           a == Axis::Y ? b.min.y : b.min.z;
}

float CollisionSystem::getMax(const AABB& b, Axis a) {
    return a == Axis::X ? b.max.x :
           a == Axis::Y ? b.max.y : b.max.z;
}
CollisionSystem::~CollisionSystem(){
    
}
