#pragma once
#include "../component/manager.h"
#include "../utils/math.h"
#include <vector>
#include <typeindex>
#include <memory>

class EntityManager {
public:
    Entity create();
    void destroy(Entity e);

    template<typename T>
    void add(Entity e, T component);

    template<typename T>
    void remove(Entity e);

    template<typename T>
    T* get(Entity e);

    template<typename... Ts>
    std::vector<Entity> view(); //find

    void build(const std::string& s, const Vec3 v = {0,0,0}); // 先create（），再添加组件

private:
    Entity nextEntity{1};
    // 每种组件类型一个容器
    std::unordered_map<std::type_index, std::unique_ptr<Pool>> pools;

    // 最外层 unordered_map
//     components[typeid(Position)]  -> Position 的组件池
// components[typeid(Velocity)]  -> Velocity 的组件池
};

template<typename T>
void EntityManager::add(Entity e, T component) {
    auto& poolBase = pools[typeid(T)];

    if (!poolBase) {
        poolBase = std::make_unique<ComponentPool<T>>();
    }

    auto* pool = static_cast<ComponentPool<T>*>(poolBase.get());
    pool->data[e] = std::move(component);
}
template<typename T>
void EntityManager::remove(Entity e) {
    auto it = pools.find(typeid(T));
    if (it == pools.end()) return;

    auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
    pool->data.erase(e);
}

template<typename T>
T* EntityManager::get(Entity e) {
    auto it = pools.find(typeid(T));
    if (it == pools.end()) return nullptr;
    auto* pool = static_cast<ComponentPool<T>*>(it->second.get());

    auto jt = pool->data.find(e);
    if (jt == pool->data.end()) return nullptr;

    return &jt->second;
}

template<typename... Ts>
std::vector<Entity> EntityManager::view() {
    std::vector<Entity> result;
    
    using First = std::tuple_element_t<0, std::tuple<Ts...>>;

    auto it = pools.find(typeid(First));
    if (it == pools.end()) return result;

    auto* firstPool = static_cast<ComponentPool<First>*>(it->second.get());
    
    for (auto& [e, _] : firstPool->data) {
        if ((get<Ts>(e) && ...)) {
            result.push_back(e);
        }
    }
    return result;
}
