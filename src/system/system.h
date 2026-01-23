#pragma once
#include <SDL.h>
#include "../entity/entity.h"
#include "../component/component.h"
class InputSystem  {
public:
    int prior = 1; //是第几个运行的系统
    InputSystem(); //默认构造函数
    void start();
    void update(EntityManager& em, float dt);
    ~InputSystem();
};

class RaycastSystem  {
public:
    int prior = 3;
    RaycastSystem (); //默认构造函数
    void start();
    void update(EntityManager& em, float dt);
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

class RenderSystem  {
public:
    int prior = 4;
    RenderSystem(); //默认构造函数
    void start();
    void update(EntityManager& em, SDL_Window* window);
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
    SystemManager(); //默认构造函数
    void start();
    void update(EntityManager& em, float dt,SDL_Window* window);
    ~SystemManager();

private:
    InputSystem*    s1 = nullptr;
    RaycastSystem* s2 = nullptr;
    RenderSystem*   s3 = nullptr;
    CollisionSystem* s4 = nullptr;
};
