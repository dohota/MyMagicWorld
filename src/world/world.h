#pragma once
#include <SDL.h>
#include <unordered_map>
#include "../entity/entity.h" 
#include "../component/component.h" 
#include <SDL_opengl.h> // macOS 上直接用这个
class World  {
public:
    SDL_Window* window;
    World(); //默认构造函数
    void start();
    void update();
    ~World();

    Entity createEntity();
    void destroyEntity(Entity e);
    template<typename T>
    void addComponent(Entity e, T component){
        if constexpr (std::is_same_v<T, Position>) {
            transforms_[e] = component;
        }
        else if constexpr (std::is_same_v<T, Velocity>) {
            velocities_[e] = component;
        }
        else {

        }
    }
    template<typename T>
    T* getComponent(Entity e) {
        if constexpr (std::is_same_v<T, Position>) {
            auto it = transforms_.find(e);
            return it != transforms_.end() ? &it->second : nullptr;
        }
        else if constexpr (std::is_same_v<T, Velocity>) {
            auto it = velocities_.find(e);
            return it != velocities_.end() ? &it->second : nullptr;
        }
        else {
            return nullptr;
        }
    }
    // template<typename T>
    // T* getComponents(Entity e) {
    //     if constexpr (std::is_same_v<T, Position>) {
    //         auto it = transforms_.find(e);
    //         return it != transforms_.end() ? &it->second : nullptr;
    //     }
    //     else if constexpr (std::is_same_v<T, Velocity>) {
    //         auto it = velocities_.find(e);
    //         return it != velocities_.end() ? &it->second : nullptr;
    //     }
    //     else {
    //         return nullptr;
    //     }
    // }  查找一个实体是否拥有某几个组件

    template<typename T>
    using ComponentMap = std::unordered_map<Entity, T>;
    // 每种组件一张表
    ComponentMap<class Position> transforms_;
    ComponentMap<class Velocity> velocities_;
    ComponentMap<class Camera> cameras_;
    ComponentMap<class Mesh> meshs_;

private:
    bool running;
    SDL_GLContext context;
    void setPerspective(float fov, float aspect, float zNear, float zFar);
    void setupProjection(int w, int h);
    // void drawCube();
    Entity nextEntity_ = 1;

};
