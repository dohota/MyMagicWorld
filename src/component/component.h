#pragma once
// component是纯数据，全写成结构体，可以用泛型模版
struct Position {
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

struct Velocity {
    float vx = 0.f;
    float vy = 0.f;
};


struct Camera {
    float yaw = -90.f;
    float pitch = 0.f;
    float fov = 70.f;
};

struct Mesh {
    unsigned int vao;
    unsigned int vertexCount;
};

struct Control {
    
}; 

struct Block {
    
}; // 单个方块

struct Chunk{
    
};//一大组方块
