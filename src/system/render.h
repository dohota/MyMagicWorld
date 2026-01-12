#pragma once
#include "../world/world.h"
#include <SDL_opengl.h> // macOS 上直接用这个
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
// 通过摄像机来决定渲染内容
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

// void renderSystem(World& world, Camera& cam) {
//     glm::mat4 view = getViewMatrix(cam);
//     glm::mat4 proj = glm::perspective(
//         glm::radians(cam.fov),
//         aspect,
//         0.1f,
//         1000.f
//     );

//     for (auto& [e, mesh] : world.meshes) {
//         auto* t = world.getComponent<Transform>(e);

//         glm::mat4 model = glm::translate(glm::mat4(1.0f), t->position);

//         shader.set("MVP", proj * view * model);
//         draw(mesh);
//     }
// }
