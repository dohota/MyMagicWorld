# include "../entity/entity.h"
# include "../component/component.h"
# include <random>
#include <cmath>
Entity EntityManager::create() {
    return nextEntity++;
}

// void EntityManager::destroy(Entity e) {
//     for (auto& [_, pool] : pools) {
//         pool->remove(e); 
//     }
// }

void EntityManager::build(const std::string& s) {
    if (s == "player"){
        Entity player = this->create();
        this->add<Position>(player, {{3, 18, 5}});
        this->add<Velocity>(player, {{0, 0, 0}});
        this->add<Camera>(player, {});
        this->add<Gravity>(player, {});
        this->add<Size>(player, {{0.3, 3, 0.3}});
        auto* pos = this->get<Position>(player);
        this->add<Collider>(player, {{{pos->position.x,pos->position.y,pos->position.z},
           {pos->position.x+0.1f,pos->position.y+0.1f,pos->position.z+0.1f}},false});
    }else if(s == "grass_block"){

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
                    this->add<Collider>(block, {{{x, static_cast<float>(y), z},
                    {x+1.5f, static_cast<float>(y)+1.5f, z+1.5f}},true});
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