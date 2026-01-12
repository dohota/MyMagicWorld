#include "../world/world.h"
#include "../component/component.h"

void inputSystem(World& world, float dt) {
    for (Entity e : world.players()) {
        auto* t = world.getComponent<Transform>(e);
        auto* v = world.getComponent<Velocity>(e);
        auto* c = world.getComponent<Camera>(e);

        glm::vec3 front{
            cos(glm::radians(c->yaw)) * cos(glm::radians(c->pitch)),
            sin(glm::radians(c->pitch)),
            sin(glm::radians(c->yaw)) * cos(glm::radians(c->pitch))
        };

        glm::vec3 right = glm::normalize(glm::cross(front, {0,1,0}));

        float speed = 5.0f;

        if (keyDown('W')) v->value += front * speed;
        if (keyDown('S')) v->value -= front * speed;
        if (keyDown('A')) v->value -= right * speed;
        if (keyDown('D')) v->value += right * speed;
    }
}
void mouseSystem(World& world, float dx, float dy) {
    for (Entity e : world.players()) {
        auto* cam = world.getComponent<Camera>(e);

        cam->yaw   += dx * 0.1f;
        cam->pitch -= dy * 0.1f;

        cam->pitch = glm::clamp(cam->pitch, -89.f, 89.f);
    }
}
