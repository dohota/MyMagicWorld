#pragma once
#include <cmath>
#include <SDL_opengl.h> // macOS 上直接用这个

// #ifndef COMPONENT_H
// #define COMPONENT_H


// #endif
struct Vec3 {
    float x, y, z;

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(float s) const { return {x*s, y*s, z*s}; }
    Vec3& operator+=(const Vec3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }

    Vec3& operator-=(const Vec3& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }
};

Vec3 cross(const Vec3& a, const Vec3& b) {
    return { a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x };
}

Vec3 normalize(const Vec3& v) {
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return {v.x/len, v.y/len, v.z/len};
}

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



struct Control {
    
}; 

struct Block {
    
}; // 单个方块

struct Chunk{
    
};//一大组方块
