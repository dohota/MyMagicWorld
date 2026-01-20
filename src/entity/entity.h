#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

using Entity = std::uint32_t;

constexpr Entity kInvalidEntity = 0;

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

    void build(const std::string& s); // 先create（），再添加组件

private:
    Entity nextEntity{1};
    // 每种组件类型一个容器
    std::unordered_map<
        std::type_index,
        std::unordered_map<Entity, std::shared_ptr<void>>
    > components;
    // 最外层 unordered_map
//     components[typeid(Position)]  -> Position 的组件池
// components[typeid(Velocity)]  -> Velocity 的组件池
};

template<typename T>
void EntityManager::add(Entity e, T component) {
    components[typeid(T)][e] =
        std::make_shared<T>(std::move(component));
}

template<typename T>
T* EntityManager::get(Entity e) {
    auto it = components.find(typeid(T));
    if (it == components.end()) return nullptr;

    auto jt = it->second.find(e);
    if (jt == it->second.end()) return nullptr;

    return static_cast<T*>(jt->second.get()); // 有概率炸
}

template<typename... Ts>
std::vector<Entity> EntityManager::view() {
    std::vector<Entity> result;

    auto& firstPool = components[typeid(
        std::tuple_element_t<0, std::tuple<Ts...>>
    )];

    for (auto& [entity, _] : firstPool) {
        if ((get<Ts>(entity) && ...)) {
            result.push_back(entity);
        }
    }
    return result;
}
