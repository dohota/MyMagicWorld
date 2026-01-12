#pragma once
#include <SDL_opengl.h> // macOS 上直接用这个
#include <SDL.h>
#include <unordered_map>
#include "../entity/entity.h" 
#include "../component/component.h" 
// 或者用 include guard

//namespace math {

class World  {
public:
    const SDL_Window* window;
    World(); //默认构造函数
    void start();
    void update();
    ~World();

    Entity createEntity();
    void destroyEntity(Entity e);
    template<typename T>
    void addComponent(Entity e, T component);
    template<typename T>
    T* getComponent(Entity e);

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
    void drawCube();
    
    Entity nextEntity_ = 1;

};
//} 
