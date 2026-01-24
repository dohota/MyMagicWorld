#include "../system/system.h"
#include <SDL_mouse.h>
//#include <iostream>
// 根据速度，和位置 决定接下来的位置
RaycastSystem  :: RaycastSystem () {
    this->prior = 3;
    this->start();
    
}
void RaycastSystem  :: start(){
    
}
void RaycastSystem::update(EntityManager& em, CommandBuffer& cv, EventBus& ev, float dt) {
    bool wantBreak = false;
    bool wantPlace = false;
    ev.subscribe<Destroy>([&](const Destroy& e){
        wantBreak = true;
    });
    ev.subscribe<Build>([&](const Build& e){
        wantPlace = true;
    });
    // 只给“有 RayCast 的实体”算（一般只有玩家）
    for (auto e : em.view<Position, RayCast>()) {
        auto* pos = em.get<Position>(e);
        auto* rc  = em.get<RayCast>(e);

        Vec3 dir = normalize(rc->ray);
        float closest = rc->max_distance;

        Entity hitEntity = kInvalidEntity;
        Vec3 hitPoint;
        Vec3 hitNormal;
        for (auto target : em.view<Collider>()) {
            if (target == e) continue;
            auto* coll = em.get<Collider>(target);
            if (!coll->isStatic) continue;
            float t;
            Vec3 normal;
            coll->box.min = pos->position + coll->localBox.min;
            coll->box.max = pos->position + coll->localBox.max;
            if (rayIntersectsAABB(pos->position, dir, coll->box,
                                  rc->max_distance, t, normal)) {
                if (t < closest) {
                    closest = t;
                    hitEntity = target; // 世界坐标命中点
                    hitPoint = pos->position + dir * t; // 命中面的法线（±X / ±Y / ±Z）
                    hitNormal = normal;
                    if (hitEntity == kInvalidEntity) continue;
                    bool leftNow  = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT);
                    bool rightNow = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT);
                    bool leftPressed  = leftNow  && !this->lastLeft;
                    bool rightPressed = rightNow && !this->lastRight;
                    this->lastLeft  = leftNow;
                    this->lastRight = rightNow;
                    
                    if (leftPressed) {
                        printf("消除了某方块\n");
                        cv.entity_destroy(hitEntity);
                    }
                    if (rightPressed) {
                        Vec3 placePos = em.get<Position>(hitEntity)->position + cross(hitNormal, Vec3{1.5f, 1.5f, 1.5f});
                        printf("创造某方块！！\n");
                        cv.entity_build(placePos,"grass_block");
                    }
                }
            }
        }
        
    }
}
bool RaycastSystem::rayIntersectsAABB(const Vec3& origin, const Vec3& dir, const AABB& box,
        float maxDist, float& outT, Vec3& outNormal) {
    float tMin = 0.0f;
    float tMax = maxDist;
    outNormal = {0, 0, 0};
    for (int axis = 0; axis < 3; axis++) {
        float o = axis == 0 ? origin.x : axis == 1 ? origin.y : origin.z;
        float d = axis == 0 ? dir.x : axis == 1 ? dir.y : dir.z;
        float minB = axis == 0 ? box.min.x : axis == 1 ? box.min.y : box.min.z;
        float maxB = axis == 0 ? box.max.x : axis == 1 ? box.max.y : box.max.z;
        if (fabs(d) < 1e-6f) {
            // 射线与 slab 平行
            if (o < minB || o > maxB)
                return false;
        } else {
            float t1 = (minB - o) / d;
            float t2 = (maxB - o) / d;

            float sign = -1.0f;
            if (t1 > t2) {
                std::swap(t1, t2);
                sign = 1.0f;
            }
            if (t1 > tMin) {
                tMin = t1;
                outNormal =
                    axis == 0 ? Vec3{sign, 0, 0} :
                    axis == 1 ? Vec3{0, sign, 0} :
                                Vec3{0, 0, sign};
            }
            tMax = std::min(tMax, t2);
            if (tMin > tMax)
                return false;
        }
    }
    outT = tMin;
    return true;
}
RaycastSystem ::~RaycastSystem (){
    
}
