#pragma once
#include <SDL.h>
#include "../entity/entity.h"
#include "../component/component.h"
#include <functional>

#include <bgfx/bgfx.h> // bgfx渲染API核心接口
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h> // bx数学工具（纹理尺寸计算等可能用到）
#include <SDL2/SDL_syswm.h>

// #include <bx/endian.h>            // bx字节序处理（纹理数据可能涉及跨平台字节序）              
// #include <bx/readerwriter.h>      // bx文件读写接口（FileReaderI等）
// #include <bx/string.h>            // bx字符串处理（文件路径操作等）
// #include <bx/debug.h>             // bx调试工具（打印错误信息等）
// #include <bimg/decode.h>             // bimg图像解码接口（解析纹理数据）
// #include <bx/allocator.h>         // bx内存分配器（管理纹理数据内存）

// 目前事件总线是即时派发的，不是队列式的
// 如果之后需要，可以再写一个队列式的事件总线
// EventBus 适合： 切换模式（飞行 / 行走），播放音效， UI 提示，粒子效果
// 不适合：destroy / build， spawn entity， 改组件结构
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
// 延迟执行的 写操作队列

// Minecraft里 所有世界修改都发生在 tick 边界：20 TPS（每秒 20 tick）
// 一个 tick 内：收集输入， 处理 AI / 物理 / 碰撞， 处理方块更新

// 如果只是简单的意图/请求，可以用event bus。要修改世界，就得用CommandBuffer
class CommandBuffer {
public:
    // put one kind of event into the event queue
    void entity_destroy(Entity entity) {
        this->destroyQueue.push_back({entity});
    }
    void entity_build(const Vec3& pos, const std::string& type) {
        this->buildQueue.push_back({pos, type});
    }
    // tick 末尾统一提交世界修改
    void flush(EntityManager& em) {
        for (auto& a : destroyQueue){ // 执行队列里的内容
            em.destroy(a.entity);
        }
        destroyQueue.clear();

        for (auto& b : buildQueue){
            em.build(b.type, b.pos);
        }
        buildQueue.clear();
    }
private:
    // event
    struct EntityDestroy {
        Entity entity;
    };
    struct EntityBuild {
        Vec3 pos;
        std::string type;
    };
    struct ChangeStatus {
        Entity entity;
        std::string old_;
        std::string new_;
    };
    // event queue
    std::vector<EntityDestroy> destroyQueue;
    std::vector<EntityBuild>  buildQueue;
};

class InputSystem  {
public:
    bool is_fly;
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
    bool lastLeft = false;
    bool lastRight = false;
    int prior = 3;
    RaycastSystem (); //默认构造函数
    void start();
    void update(EntityManager& em, CommandBuffer& cv, EventBus& ev, float dt);
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
    EntitySystem();
    void start();
    void update(EntityManager& em, CommandBuffer& cv, EventBus& ev, float dt);
    ~EntitySystem();
};

class RenderSystem  {
public:
    int prior = 4;
    RenderSystem(SDL_Window* window); //默认构造函数
    void start();
    void update(EntityManager& em, EventBus& ev);
    ~RenderSystem();
private:
    SDL_Window* window;
    bgfx::ProgramHandle defaultProgram;
    void drawCube(float x, float y, float z, float s);
    void drawCrosshair(int screenWidth, int screenHeight, float size = 10.0f, float thickness = 1.0f); // 十字准星
    bool isAABBVisible(const Vec3& camPos, const Vec3& camFront, const Vec3& camUp,
                   float fov, float aspect, float nearDist, float farDist, const AABB& box);
    
};

class SystemManager  {
public:
    SystemManager(SDL_Window* window); //默认构造函数
    void start();
    void update(EntityManager& em,CommandBuffer& cv, EventBus& ev, float dt);
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

// event bus中的事件
struct FlyMode { // 双击空格
};
struct Build { // 鼠标左键
};
struct Destroy { // 鼠标右键
};
