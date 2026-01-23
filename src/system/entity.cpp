#include "../system/system.h"

EntitySystem :: EntitySystem(EntityManager& em, EventBus& ev) {
    this->prior = 4;
    ev.subscribe<EntityDestroy>([&](const EntityDestroy& e){
        em.destroy(e.entity);
    });
    ev.subscribe<EntityBuild>([&](const EntityBuild& e){
        em.build(e.type, e.position);
    });
    //this->start();
}
void EntitySystem :: start(EntityManager& em, EventBus& ev){
    
}
void EntitySystem :: update(EntityManager& em, EventBus& ev, float dt)  {
    
}
EntitySystem::~EntitySystem(){
    
}
