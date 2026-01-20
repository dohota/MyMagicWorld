#include "../system/system.h"
#include "../component/component.h"
#include <SDL.h>
#include <cmath>
// #include <iostream>
InputSystem :: InputSystem() {
    this->prior = 1;
    this->start();
}
void InputSystem :: start(){
    
}
void InputSystem :: update(EntityManager& em, float dt)  {
    SDL_PumpEvents();//虽然调用了 SDL_PollEvent，但在某些情况下（特别是 macOS）必须保证这一句在前面
    int dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    // 鼠标灵敏度
    float sensitivity = 0.1f;
    // 移动速度（单位/秒）
    float speed = 5.0f; 

    for (auto e : em.view<Position, Velocity,Camera>()) {
        auto* pos = em.get<Position>(e);
        auto* vel = em.get<Velocity>(e);
        auto* cam = em.get<Camera>(e);
        if (!pos || !vel || !cam) continue;

        // ====== 鼠标控制视角 ======
        cam->yaw   += dx * sensitivity;
        cam->pitch -= dy * sensitivity;

        if (cam->pitch > 89.f) cam->pitch = 89.f;
        if (cam->pitch < -89.f) cam->pitch = -89.f;
        //new code
        cam->yaw = fmodf(cam->yaw, 360.0f);
        if (cam->yaw < 0) cam->yaw += 360.0f;
        // 计算摄像机前向和右向
        float yawRad = cam->yaw * M_PI/180.0f;
        float pitchRad = cam->pitch * M_PI/180.0f;
        // ====== 摄像机方向======
        Vec3 camFront{
            cosf(pitchRad) * sinf(yawRad),
            sinf(pitchRad),
            -cosf(pitchRad) * cosf(yawRad)
        }; // 在“看哪里”
        camFront = normalize(camFront);
        
        // ====== 移动方向（只在 XZ） ======
        Vec3 moveFront{
            sinf(yawRad),
            0.0f,
            -cosf(yawRad)
        }; // 在“地面上往哪走”
        moveFront = normalize(moveFront);
        Vec3 moveRight = normalize(cross(moveFront, Vec3{0,1,0}));
        // ====== WASD ======
        //vel->value = {0,0,0}; // 先清零
        if (state[SDL_SCANCODE_W]) vel->value += moveFront * speed;
        if (state[SDL_SCANCODE_S]) vel->value -= moveFront * speed;
        if (state[SDL_SCANCODE_A]) vel->value -= moveRight * speed;
        if (state[SDL_SCANCODE_D]) vel->value += moveRight * speed;
        // ====== 上下移动 ======
        if (state[SDL_SCANCODE_SPACE]) vel->value.y += speed ;
        if (state[SDL_SCANCODE_LSHIFT]) vel->value.y -= speed ;
        if (state[SDL_SCANCODE_P]) printf("change fly/walk");
    }
}
InputSystem::~InputSystem(){
    
}
