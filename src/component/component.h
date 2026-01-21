#pragma once
#include "../utils/math.h"
#include <SDL_opengl.h> // macOS 上直接用这个
struct Position {
    Vec3 position;
};

struct Velocity {
    Vec3 value;
};

struct Camera {
    float yaw = -90.f;
    float pitch = 0.f;
    float fov = 70.f;

    Vec3 front() const {
        float cy = cosf(yaw * M_PI/180.0f);
        float sy = sinf(yaw * M_PI/180.0f);
        float cp = cosf(pitch * M_PI/180.0f);
        float sp = sinf(pitch * M_PI/180.0f);

        return normalize(Vec3{cy*cp, sp, sy*cp});
    }
};

struct Mesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    int vertexCount = 0;
};

enum class Axis {
    X,
    Y,
    Z
};
struct Gravity{
    Vec3 gravity = {0, -9.8f, 0};
};
struct Size {
    Vec3 value;
};
struct AABB {
    Vec3 min; // 最小坐标 (x, y, z)
    Vec3 max; // 最大坐标
    // 平移（原地修改）
    void offset(float dx, float dy, float dz) {
        min.x += dx; max.x += dx;
        min.y += dy; max.y += dy;
        min.z += dz; max.z += dz;
    }

    // 返回中心点（给 Position 用）
    Vec3 getCenter() const {
        return (min + max) * 0.5f;
    }
};
struct Collider {// 碰撞体
    AABB box; // aabb碰撞体
    bool isStatic; // true 表示静态方块，不会移动
};
// 玩家：isStatic = false
// 地形方块：isStatic = true
struct Ray {
    Vec3 origin;
    Vec3 dir; // 必须 normalize
};
// 射线 origin + t*dir 与 AABB 的相交测试，用于 方块拾取/攻击/瞄准

struct GroupPosition{
    
};
struct GroupSize{
    
};
