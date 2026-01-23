# include "../entity/entity.h"
# include "../component/component.h"
# include <random>
#include <cmath>
Entity EntityManager::create() {
    return nextEntity++;
}

void EntityManager::destroy(Entity e) {
    if (e == kInvalidEntity) return;
    for (auto& [type, pool] : pools) {
        // 我们知道所有 pool 实际类型都是 ComponentPool<T>
        // 所以可以用 void* + 模板技巧
        auto* base = pool.get();

        // 用一个通用 lambda 擦除
        // 关键：unordered_map::erase 不存在 key 时是安全的
        if (auto* p = dynamic_cast<ComponentPool<Position>*>(base)) {
            p->data.erase(e);
        }
        if (auto* p = dynamic_cast<ComponentPool<Velocity>*>(base)) {
            p->data.erase(e);
        }
        if (auto* p = dynamic_cast<ComponentPool<Collider>*>(base)) {
            p->data.erase(e);
        }
        if (auto* p = dynamic_cast<ComponentPool<RayCast>*>(base)) {
            p->data.erase(e);
        }
        if (auto* p = dynamic_cast<ComponentPool<Camera>*>(base)) {
            p->data.erase(e);
        }
        if (auto* p = dynamic_cast<ComponentPool<Gravity>*>(base)) {
            p->data.erase(e);
        }
        if (auto* p = dynamic_cast<ComponentPool<Size>*>(base)) {
            p->data.erase(e);
        }
        if (auto* p = dynamic_cast<ComponentPool<AABB>*>(base)) {
            p->data.erase(e);
        }
    }
}

void EntityManager::build(const std::string& s, const Vec3 v) {
    if (s == "player"){
        Entity player = this->create();
        this->add<Position>(player, {{3, 18, 5}});
        this->add<Velocity>(player, {{0, 0, 0}});
        this->add<Camera>(player, {});
        this->add<Gravity>(player, {});
        this->add<Size>(player, {{0.3, 3, 0.3}});
        this->add<RayCast>(player,{{-90,0,70},100});
        auto* pos = this->get<Position>(player);
        Collider playerCollider;
        playerCollider.localBox = {
            {-0.3f, 0.0f, -0.3f},
            { 0.3f, 1.8f,  0.3f} // position是脚底中间的位置
        };
        playerCollider.isStatic = false;
        this->add<Collider>(player, playerCollider);
    }else if(s == "grass_block"){
        Entity b = this->create();
        this->add<Position>(b, {{v.x, v.y, v.z}});
        this->add<Size>(b, {{1.5, 1.5, 1.5}});
        auto* pos = this->get<Position>(b);
        Collider collider;
        collider.localBox = {
            {-0.75f, 0.0f, -0.75f},
            { 0.75f, 1.5f,  0.75f}
        };
        collider.isStatic = true;
        this->add<Collider>(b, collider);
    }else if(s == "grass_chunk"){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> noise(-1.0f, 1.0f);
        float block_size = 1.5f;
        for (float x = -20; x <= 10; x += block_size) {
            for (float z = -5; z <= 14; z += block_size) {
                // 用整数坐标采样噪声（关键）
                float h =
                    std::sin(x * 0.25f) +
                    std::cos(z * 0.25f) +
                    noise(gen) * 0.5f;
                int height = static_cast<int>(std::floor(h * 3 + 5));
                height = std::max(1, height); // 防止负数
                for (int y = 0; y <= height; y++){
                    Entity block = this->create();
                    this->add<Position>(block, {{x, static_cast<float>(y), z}});
                    this->add<Size>(block, {{1.5, 1.5, 1.5}});
                    auto* pos = this->get<Position>(block);
                    Collider collider;
                    collider.localBox = {
                        {-0.75f, 0.0f, -0.75f},
                        { 0.75f, 1.5f,  0.75f}
                    };
                    collider.isStatic = true;
                    this->add<Collider>(block, collider);
                }
            }
        }
    }
}
// for x in 整数
//   for z in 整数
//     height = noise(x, z)   // 整数
//     for y = 0..height
//       放方块 (x, y, z)