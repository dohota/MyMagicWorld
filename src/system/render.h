#pragma once
#include "../world/world.h"
float dot(const Vec3& a, const Vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
void lookAt(const Vec3& eye, const Vec3& center, const Vec3& up, float* out) {
    Vec3 f = normalize(center - eye);
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    out[0] = s.x; out[4] = s.y; out[8] = s.z;  out[12] = -dot(s, eye);
    out[1] = u.x; out[5] = u.y; out[9] = u.z;  out[13] = -dot(u, eye);
    out[2] = -f.x; out[6] = -f.y; out[10] = -f.z; out[14] = dot(f, eye);
    out[3] = 0; out[7] = 0; out[11] = 0; out[15] = 1;
}
void perspective(float fov, float aspect, float zNear, float zFar, float* out) {
    float tanHalfFovy = tanf(fov * M_PI/360.f);
    for(int i=0;i<16;i++) out[i]=0;
    out[0] = 1/(aspect*tanHalfFovy);
    out[5] = 1/tanHalfFovy;
    out[10] = -(zFar+zNear)/(zFar-zNear);
    out[11] = -1;
    out[14] = -(2*zFar*zNear)/(zFar-zNear);
}
// 生成 4x4 平移矩阵
void translateMatrix(const Vec3& pos, float out[16]) {
    // 单位矩阵
    for(int i=0;i<16;i++) out[i] = 0.f;
    out[0] = 1.f; out[5] = 1.f; out[10] = 1.f; out[15] = 1.f;
    // 设置平移
    out[12] = pos.x;
    out[13] = pos.y;
    out[14] = pos.z;
}
// out = a * b * c （3个 4x4 矩阵相乘）
void multiplyMatrices(const float a[16], const float b[16], const float c[16], float out[16]) {
    float temp[16];
    // 先 a * b
    for(int row=0; row<4; row++){
        for(int col=0; col<4; col++){
            temp[col + row*4] = 0.f;
            for(int k=0; k<4; k++){
                temp[col + row*4] += a[k + row*4] * b[col + k*4];
            }
        }
    }
    // 再 temp * c
    for(int row=0; row<4; row++){
        for(int col=0; col<4; col++){
            out[col + row*4] = 0.f;
            for(int k=0; k<4; k++){
                out[col + row*4] += temp[k + row*4] * c[col + k*4];
            }
        }
    }
} // 注意 OpenGL 默认是 column-major，所以索引用 col + row*4

// void createCubeMesh(Mesh& mesh) {
//     float vertices[] = {
//         // 只放一个立方体的简单顶点
//         -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,0.5f,-0.5f,
//         0.5f,0.5f,-0.5f,  -0.5f,0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
//         // ...其余 5 面
//     };
//     mesh.vertexCount = sizeof(vertices)/sizeof(float)/3;

//     glGenVertexArrays(1, &mesh.vao);
//     glGenBuffers(1, &mesh.vbo);

//     glBindVertexArray(mesh.vao);
//     glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);
// }
//渲染的时候只要：
//glBindVertexArray(mesh.vao);
//glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);

void renderSystem(World& world) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 找到摄像机
    Position* camPos = nullptr;
    Camera* cam = nullptr;
    for (auto& [e, c] : world.cameras_) {
        cam = &c;
        camPos = world.getComponent<Position>(e);
        if (camPos) break;
    }
    if (!cam || !camPos) return;

    float view[16];
    lookAt(camPos->position, camPos->position + cam->front(), Vec3{0,1,0}, view);
    float proj[16];
    perspective(cam->fov, 1280.f/720.f, 0.1f, 1000.f, proj);

    for (auto& [e, mesh] : world.meshs_) {
        auto* pos = world.getComponent<Position>(e);
        if (!pos) continue;

        float model[16];
        translateMatrix(pos->position, model); // 手写 translate 矩阵
        float mvp[16];
        multiplyMatrices(proj, view, model, mvp);

        glUniformMatrix4fv(shaderMVP, 1, GL_FALSE, mvp);
        glBindVertexArray(mesh.vao);
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    }

    SDL_GL_SwapWindow(world.window);
}
