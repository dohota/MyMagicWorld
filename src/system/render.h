#pragma once
#include "../world/world.h"
//SDL2 + OpenGL 旧式（Immediate Mode）

// OpenGL 本身是一个渲染管线，它不会自己存储场景里物体的位置或逻辑关系，它只知道你给它 顶点坐标 和 变换矩阵。

// 为了方便处理场景，你通常会把场景中的物体放到一个 统一坐标系 中，这就是 世界坐标系 (World Space)。

// 世界坐标系：整个场景的参考坐标系。比如你做一个 Minecraft 世界，地面在 y=0，树在 (x=5, y=0, z=3)，所有物体的位置都相对于这个坐标系来描述。

// 局部坐标系 (Local/Object Space)：每个物体自己的坐标系。比如一个立方体的顶点坐标可能是 (0,0,0) 到 (1,1,1)，这是它自身的局部坐标
//-----
// 世界 = 你的场景地图，所有物体都有自己的“地图坐标”

// 模型矩阵 = 把物体放到地图上的位置

// 视图矩阵 = 摄像机站在哪看地图

// 投影矩阵 = 摄像机镜头把 3D 映射成 2D
inline void drawCube(float x, float y, float z, float s = 0.2f) {
    glBegin(GL_QUADS);
    // 前面 (z+s)
    glColor3f(0.3f, 0.8f, 0.3f); // 绿色
    glVertex3f(x,   y,   z+s);
    glVertex3f(x+s, y,   z+s);
    glVertex3f(x+s, y+s, z+s);
    glVertex3f(x,   y+s, z+s);
    // 后面 (z)
    glColor3f(0.3f, 0.5f, 0.3f); // 深绿色
    glVertex3f(x,   y,   z);
    glVertex3f(x+s, y,   z);
    glVertex3f(x+s, y+s, z);
    glVertex3f(x,   y+s, z);
    // 左面 (x)
    glColor3f(0.25f, 0.7f, 0.25f);
    glVertex3f(x, y,   z);
    glVertex3f(x, y,   z+s);
    glVertex3f(x, y+s, z+s);
    glVertex3f(x, y+s, z);
    // 右面 (x+s)
    glColor3f(0.25f, 0.6f, 0.25f);
    glVertex3f(x+s, y,   z);
    glVertex3f(x+s, y,   z+s);
    glVertex3f(x+s, y+s, z+s);
    glVertex3f(x+s, y+s, z);
    // 上面 (y+s)
    glColor3f(0.4f, 0.9f, 0.4f);
    glVertex3f(x,   y+s, z);
    glVertex3f(x+s, y+s, z);
    glVertex3f(x+s, y+s, z+s);
    glVertex3f(x,   y+s, z+s);
    // 下面 (y)
    glColor3f(0.2f, 0.5f, 0.2f);
    glVertex3f(x,   y, z);
    glVertex3f(x+s, y, z);
    glVertex3f(x+s, y, z+s);
    glVertex3f(x,   y, z+s);
    // --- 再绘制边框 ---
    glColor3f(0.0f, 0.0f, 0.0f); // 黑色边框
    glLineWidth(20);           // 边框线宽
    glBegin(GL_LINES);

    // 8个顶点
    float x0 = x,   y0 = y,   z0 = z;
    float x1 = x+s, y1 = y+s, z1 = z+s;

    // 前面边
    glVertex3f(x0, y0, z1); glVertex3f(x1, y0, z1);
    glVertex3f(x1, y0, z1); glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z1); glVertex3f(x0, y1, z1);
    glVertex3f(x0, y1, z1); glVertex3f(x0, y0, z1);

    // 后面边
    glVertex3f(x0, y0, z0); glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z0); glVertex3f(x1, y1, z0);
    glVertex3f(x1, y1, z0); glVertex3f(x0, y1, z0);
    glVertex3f(x0, y1, z0); glVertex3f(x0, y0, z0);

    // 连接前后顶点
    glVertex3f(x0, y0, z0); glVertex3f(x0, y0, z1);
    glVertex3f(x1, y0, z0); glVertex3f(x1, y0, z1);
    glVertex3f(x1, y1, z0); glVertex3f(x1, y1, z1);
    glVertex3f(x0, y1, z0); glVertex3f(x0, y1, z1);
    glEnd();
}


inline void gluPerspective(float fov, float aspect, float zNear, float zFar) {
    float f = 1.0f / tanf(fov * 0.5f * 3.1415926f / 180.0f);

    float m[16] = {
        f / aspect, 0, 0,  0,
        0, f, 0,  0,
        0, 0, (zFar+zNear)/(zNear-zFar), -1,
        0, 0, (2*zFar*zNear)/(zNear-zFar), 0
    };

    glMultMatrixf(m);
}

inline void render_system(World& world) {
    glEnable(GL_DEPTH_TEST);

    int w, h;
    SDL_GetWindowSize(world.window, &w, &h);

    glViewport(0, 0, w, h);

    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (float)w / h, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //把当前矩阵重置为“单位矩阵”
    glTranslatef(0, 0, -15);//把整个世界移动 便于观察

    //1️⃣ 找摄像机
    Position* camPos = nullptr;
    Camera* cam = nullptr;
    for (auto& [e, c] : world.cameras_) {
        cam = &c;
        camPos = world.getComponent<Position>(e);
        break;
    }
    if (cam && camPos) {
        // 2️⃣ 反向移动世界 = 摄像机
        glRotatef(-cam->pitch, 1, 0, 0);
        glRotatef(-cam->yaw,   0, 1, 0);
        glTranslatef( //把整个世界移动 便于观察
            -camPos->position.x,
            -camPos->position.y,
            -camPos->position.z
        );
    }
    // 3.渲染一堆方块
    for (auto& [e, pos] : world.transforms_) {
        if(pos.position.z!= 5){ //玩家的位置不能当作方块渲染
            drawCube(pos.position.x, pos.position.y, pos.position.z);
        }
        
    }
    SDL_GL_SwapWindow(world.window);
}
