#pragma once
#include <unordered_map>
#include <cstdint>
//#include <vector>
using Entity = std::uint32_t;

constexpr Entity kInvalidEntity = 0;

struct Pool {
    virtual ~Pool() = default;
    //virtual void remove(Entity e) = 0;
};

template<typename T>
struct ComponentPool : Pool {
    std::unordered_map<Entity, T> data;
};
// template<typename T>
// struct ComponentPool : Pool {
//     std::vector<Entity> entities;
//     std::vector<T> data;
//     std::unordered_map<Entity, size_t> index;

//     void remove(Entity e) override {
//         auto it = index.find(e);
//         if (it == index.end()) return;

//         size_t i = it->second;
//         size_t last = data.size() - 1;

//         // swap-remove
//         data[i] = std::move(data[last]);
//         entities[i] = entities[last];
//         index[entities[i]] = i;

//         data.pop_back();
//         entities.pop_back();
//         index.erase(it);
//     }
// };

