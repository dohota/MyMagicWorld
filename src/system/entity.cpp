#include "../system/system.h"

EntitySystem :: EntitySystem() {
    this->prior = 6;
    this->start();
}
void EntitySystem :: start(){
    
}
void EntitySystem :: update(EntityManager& em, CommandBuffer& cv, EventBus& ev, float dt)  {
    // ev.subscribe<EntityDestroy>([&](const EntityDestroy& e){
    //     em.destroy(e.entity);
    // });
    // ev.subscribe<EntityBuild>([&](const EntityBuild& e){
    //     em.build(e.type, e.position);
    // });

    // cv.flush(em);
}
EntitySystem::~EntitySystem(){
    
}
