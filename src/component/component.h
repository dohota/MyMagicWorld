#pragma once
#include "../utils/math.h"
#include <SDL_opengl.h> // macOS 上直接用这个

// #ifndef COMPONENT_H
// #define COMPONENT_H


// #endif
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
