#include "../system/system.h"
#include "../component/component.h"
#include <SDL.h>
#include <cmath>

InputSystem :: InputSystem() {
    this->prior = 1;
    this->start();
}
void InputSystem :: start(){
    
}
void InputSystem :: update(World& world, float dt)  {
    SDL_PumpEvents();//虽然调用了 SDL_PollEvent，但在某些情况下（特别是 macOS）必须保证这一句在前面
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
            // cosf(yawRad) * cosf(pitchRad),
            // sinf(pitchRad),
            // sinf(yawRad) * cosf(pitchRad)
            cosf(pitchRad) * sinf(yawRad),  // X
            sinf(pitchRad),                  // Y
            -cosf(pitchRad) * cosf(yawRad)  // Z
            //这样 yaw=0 时朝 -Z 方向，yaw=90° 时朝 +X，符合默认 OpenGL
        };
        Vec3 right = normalize(cross(front, Vec3{0,1,0}));

        float speed = 5.f;

        if (state[SDL_SCANCODE_W]) vel->value += front * speed;
        if (state[SDL_SCANCODE_S]) vel->value -= front * speed;
        if (state[SDL_SCANCODE_A]) vel->value -= right * speed;
        if (state[SDL_SCANCODE_D]) vel->value += right * speed;
        // 鼠标控制摄像机角度
        cam.yaw   += dx * 0.1f;
        cam.pitch -= dy * 0.1f;
        if (cam.pitch > 89.f) cam.pitch = 89.f;
        if (cam.pitch < -89.f) cam.pitch = -89.f;
        //new code
        cam.yaw = fmodf(cam.yaw, 360.0f);
        if (cam.yaw < 0) cam.yaw += 360.0f;
    }
}
InputSystem::~InputSystem(){
    
}
