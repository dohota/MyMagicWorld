#pragma once
#include "../world/world.h"
#include "../component/component.h"
#include <SDL.h>
#include <cmath>

void inputSystem(World& world, float dt) {
    int dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto& [e, cam] : world.cameras_) {
        auto* pos = world.getComponent<Position>(e);
        auto* vel = world.getComponent<Velocity>(e);
        if (!pos || !vel) continue;

        // 计算摄像机前向和右向
        float yawRad = cam.yaw * M_PI/180.0f;
        float pitchRad = cam.pitch * M_PI/180.0f;

        Vec3 front{
            cosf(yawRad) * cosf(pitchRad),
            sinf(pitchRad),
            sinf(yawRad) * cosf(pitchRad)
        };
        Vec3 right = normalize(cross(front, Vec3{0,1,0}));

        float speed = 5.f;

        if (state[SDL_SCANCODE_W]) vel->value += front * speed * dt;
        if (state[SDL_SCANCODE_S]) vel->value -= front * speed * dt;
        if (state[SDL_SCANCODE_A]) vel->value -= right * speed * dt;
        if (state[SDL_SCANCODE_D]) vel->value += right * speed * dt;

        // 鼠标控制摄像机角度
        cam.yaw   += dx * 0.1f;
        cam.pitch -= dy * 0.1f;
        if (cam.pitch > 89.f) cam.pitch = 89.f;
        if (cam.pitch < -89.f) cam.pitch = -89.f;
    }
}

// void inputSystem(World& world, float dt) {
//     for (Entity e : world.players()) {
//         auto* t = world.getComponent<Transform>(e);
//         auto* v = world.getComponent<Velocity>(e);
//         auto* c = world.getComponent<Camera>(e);

//         glm::vec3 front{
//             cos(glm::radians(c->yaw)) * cos(glm::radians(c->pitch)),
//             sin(glm::radians(c->pitch)),
//             sin(glm::radians(c->yaw)) * cos(glm::radians(c->pitch))
//         };

//         glm::vec3 right = glm::normalize(glm::cross(front, {0,1,0}));

//         float speed = 5.0f;

//         if (keyDown('W')) v->value += front * speed;
//         if (keyDown('S')) v->value -= front * speed;
//         if (keyDown('A')) v->value -= right * speed;
//         if (keyDown('D')) v->value += right * speed;
//     }
// }
// void mouseSystem(World& world, float dx, float dy) {
//     for (Entity e : world.players()) {
//         auto* cam = world.getComponent<Camera>(e);

//         cam->yaw   += dx * 0.1f;
//         cam->pitch -= dy * 0.1f;

//         cam->pitch = glm::clamp(cam->pitch, -89.f, 89.f);
//     }
// }

// void inputSystem(World& world, float dt) {
//     int dx, dy;
//     SDL_GetRelativeMouseState(&dx, &dy);

//     const Uint8* state = SDL_GetKeyboardState(nullptr);

//     for (auto& [e, cam] : world.cameras_) {
//         auto* t = world.getComponent<Position>(e);
//         auto* v = world.getComponent<Velocity>(e);
//         if (!t || !v) continue;

//         Vec3 f = cam.front();
//         Vec3 r = normalize(cross(f, Vec3{0,1,0}));

//         float speed = 5.f;
//         if (state[SDL_SCANCODE_W]) v->value += f * speed * dt;
//         if (state[SDL_SCANCODE_S]) v->value += f * -speed * dt;
//         if (state[SDL_SCANCODE_A]) v->value += r * -speed * dt;
//         if (state[SDL_SCANCODE_D]) v->value += r * speed * dt;

//         cam.yaw   += dx * 0.1f;
//         cam.pitch -= dy * 0.1f;
//         if (cam.pitch > 89.f) cam.pitch = 89.f;
//         if (cam.pitch < -89.f) cam.pitch = -89.f;
//     }
// }
