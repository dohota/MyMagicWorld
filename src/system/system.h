#pragma once
#include <SDL.h>
#include "../entity/entity.h"
#include "../component/component.h"
#include <functional>
// 目前事件总线是即时派发的，不是队列式的
// 如果之后需要，可以再写一个队列式的事件总线
class EventBus {
public:
    EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    template<typename Event>
    void emit(const Event& e) {
        auto& handlers = getHandlers<Event>();
        for (auto& h : handlers){
            h(e);
        }   
    }
    template<typename Event>
    void subscribe(std::function<void(const Event&)> handler) {
        getHandlers<Event>().push_back(handler);
    }

private:
    template<typename Event>
    std::vector<std::function<void(const Event&)>>& getHandlers() {
        static std::vector<std::function<void(const Event&)>> handlers;
        return handlers;
    }
};

class InputSystem  {
public:
    int prior = 1; //是第几个运行的系统
    InputSystem(); //默认构造函数
    void start();
    void update(EntityManager& em, EventBus& ev, float dt);
    ~InputSystem();
    float spaceTimer = 0.0f;
    int spaceCount = 0;
};

class RaycastSystem  {
public:
    int prior = 3;
    RaycastSystem (); //默认构造函数
    void start();
    void update(EntityManager& em, EventBus& ev, float dt);
    ~RaycastSystem ();
private:
    bool rayIntersectsAABB(
    const Vec3& origin, const Vec3& dir,       // 必须是单位向量
    const AABB& box,       // 世界坐标 AABB
    float maxDist, float& outT,           // 输出：命中距离
    Vec3& outNormal );       // 输出：命中面的法线
};

class CollisionSystem{
public:
    int prior = 2;
    CollisionSystem();
    void start();
    void update(EntityManager& em, float dt);
    ~CollisionSystem();
private:
    float resolveAxis(EntityManager& em,Entity self,const AABB& box,float d, Axis axis);
    float clip(const AABB& a,const AABB& b,float d,Axis axis);
    bool overlapsOnOtherAxes(const AABB& a,const AABB& b,Axis axis);
    float getMin(const AABB& b, Axis a);
    float getMax(const AABB& b, Axis a);
};

class EntitySystem{
public:
    int prior = 4;
    EntitySystem(EntityManager& em, EventBus& ev);
    void start(EntityManager& em, EventBus& ev);
    void update(EntityManager& em, EventBus& ev, float dt);
    ~EntitySystem();
};

class RenderSystem  {
public:
    int prior = 4;
    RenderSystem(); //默认构造函数
    void start();
    void update(EntityManager& em, EventBus& ev, SDL_Window* window);
    ~RenderSystem();
private:
    void drawCube(float x, float y, float z, float s);
    void gluPerspective(float fov, float aspect, float zNear, float zFar);
    void drawCrosshair(int screenWidth, int screenHeight, float size = 10.0f, float thickness = 1.0f); // 十字准星
    bool isAABBVisible(const Vec3& camPos, const Vec3& camFront, const Vec3& camUp,
                   float fov, float aspect, float nearDist, float farDist, const AABB& box);
};

class SystemManager  {
public:
    SystemManager(EntityManager& em, EventBus& ev); //默认构造函数
    void start();
    void update(EntityManager& em, EventBus& ev, float dt,SDL_Window* window);
    ~SystemManager();
private:
    InputSystem*    s1 = nullptr;
    RaycastSystem* s2 = nullptr;
    RenderSystem*   s3 = nullptr;
    CollisionSystem* s4 = nullptr;
    EntitySystem* s5 = nullptr;
};

class SystemScheduler { // 控制系统执行顺序————底层就是决定哪些并行，多线程调度啥的。可以将其代码合并到system manager里
    std::vector<std::function<void(float)>> systems;
public:
    void addSystem(std::function<void(float)> sys) { systems.push_back(sys); }
    void update(float dt) { for (auto& s : systems) s(dt); }
};

// 事件总线中的事件
struct FlyMode { // 双击空格
};
struct Build { // 鼠标左键
};
struct Destroy { // 鼠标右键
};

struct EntityDestroy {
    Entity entity;
};

struct EntityBuild {
    Vec3 position;
    std::string type;
};
