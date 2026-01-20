# include "../entity/entity.h"
# include "../component/component.h"
Entity EntityManager::create() {
    return nextEntity++;
}

void EntityManager::destroy(Entity e) {
    for (auto& [_, pool] : components) {
        pool.erase(e);
    }
}

void EntityManager::build(const std::string& s) {
    if (s == "player"){
        Entity player = this->create();
        this->add<Position>(player, {{0, 2, 5}});
        this->add<Velocity>(player, {{0, 0, 0}});
        this->add<Camera>(player, {});
    }else if(s == "grass_block"){

    }else if(s == "grass_chunk"){
        for (float x = -20; x <= 10; x+=1.5) {
            for (float y = -10; y <= 3; y+=1.5){
                for (float z = -5; z <= 14; z+=1.5) {
                    Entity block = this->create();
                    this->add<Position>(block, {{x, y, z}});
                }
            }
        }
    }
}
