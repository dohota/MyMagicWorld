#pragma once
#include "../world/world.h"
//SDL2 + OpenGL 旧式（Immediate Mode）
void drawCube(float x, float y, float z) {
    float s = 1.0f;
    glBegin(GL_QUADS);

    glColor3f(0.3f, 0.8f, 0.3f);
    glVertex3f(x,   y,   z);
    glVertex3f(x+s, y,   z);
    glVertex3f(x+s, y+s, z);
    glVertex3f(x,   y+s, z);

    glEnd();
}

void render_system(World& world) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto& [e, pos] : world.transforms_) {
        drawCube(pos.position.x, pos.position.y, pos.position.z);
    }
    SDL_GL_SwapWindow(world.window);
}
