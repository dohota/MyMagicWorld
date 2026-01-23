#include "../system/system.h"
#include "../component/component.h"
#include "../utils/math.h"
#include <SDL_opengl.h> // macOS 上直接用这个
//#include <iostream>

//SDL2 + OpenGL 旧式（Immediate Mode）
// OpenGL 本身是一个渲染管线，它不会自己存储场景里物体的位置或逻辑关系，它只知道你给它 顶点坐标 和 变换矩阵
// 为了方便处理场景，你通常会把场景中的物体放到一个 统一坐标系 中，这就是 世界坐标系 (World Space)

// 世界坐标系：整个场景的参考坐标系。比如你做一个 Minecraft 世界，地面在 y=0，树在 (x=5, y=0, z=3)，所有物体的位置都相对于这个坐标系来描述
// 局部坐标系 (Local/Object Space)：每个物体自己的坐标系。比如一个立方体的顶点坐标可能是 (0,0,0) 到 (1,1,1)，这是它自身的局部坐标
//-----
// 世界 = 你的场景地图，所有物体都有自己的“地图坐标”
// 模型矩阵 = 把物体放到地图上的位置
// 视图矩阵 = 摄像机站在哪看地
// 投影矩阵 = 摄像机镜头把 3D 映射成 2D
void RenderSystem :: drawCrosshair(int screenWidth, int screenHeight, float size, float thickness) {
    // 保存矩阵状态
    glPushMatrix();
    
    // 使用正交投影，让坐标就是屏幕像素
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 设置颜色
    glColor3f(1.0f, 1.0f, 1.0f); // 白色

    // 屏幕中心
    float cx = screenWidth  / 2.0f;
    float cy = screenHeight / 2.0f;

    glLineWidth(thickness);
    glBegin(GL_LINES);
        // 水平线
        glVertex2f(cx - size, cy);
        glVertex2f(cx + size, cy);
        // 垂直线
        glVertex2f(cx, cy - size);
        glVertex2f(cx, cy + size);
    glEnd();

    // 恢复矩阵
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void RenderSystem :: drawCube(float x, float y, float z, float s) {
    // --- 绘制面片 ---
    glEnable(GL_POLYGON_OFFSET_FILL);      // 开启深度偏移
    glPolygonOffset(1.0f, 1.0f);          // 防止线条 z-fighting
    glBegin(GL_QUADS);
    
    // 所有面颜色统一为绿色
    glColor3f(0.3f, 0.8f, 0.3f);

    // 前面 (z+s)
    glVertex3f(x,   y,   z+s);
    glVertex3f(x+s, y,   z+s);
    glVertex3f(x+s, y+s, z+s);
    glVertex3f(x,   y+s, z+s);

    // 后面 (z)
    glVertex3f(x,   y,   z);
    glVertex3f(x+s, y,   z);
    glVertex3f(x+s, y+s, z);
    glVertex3f(x,   y+s, z);

    // 左面 (x)
    glVertex3f(x, y,   z);
    glVertex3f(x, y,   z+s);
    glVertex3f(x, y+s, z+s);
    glVertex3f(x, y+s, z);

    // 右面 (x+s)
    glVertex3f(x+s, y,   z);
    glVertex3f(x+s, y,   z+s);
    glVertex3f(x+s, y+s, z+s);
    glVertex3f(x+s, y+s, z);

    // 上面 (y+s)
    glVertex3f(x,   y+s, z);
    glVertex3f(x+s, y+s, z);
    glVertex3f(x+s, y+s, z+s);
    glVertex3f(x,   y+s, z+s);

    // 下面 (y)
    glVertex3f(x,   y, z);
    glVertex3f(x+s, y, z);
    glVertex3f(x+s, y, z+s);
    glVertex3f(x,   y, z+s);

    glEnd(); // cannot delete this
    glDisable(GL_POLYGON_OFFSET_FILL);

    // --- 绘制边框 ---
    glColor3f(0.0f, 0.0f, 0.0f); // 黑色边框
    glLineWidth(1.0f);
    glBegin(GL_LINES);

    // 八个顶点
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

void RenderSystem :: gluPerspective(float fov, float aspect, float zNear, float zFar) {
    float f = 1.0f / tanf(fov * 0.5f * 3.1415926f / 180.0f);

    float m[16] = {
        f / aspect, 0, 0,  0,
        0, f, 0,  0,
        0, 0, (zFar+zNear)/(zNear-zFar), -1,
        0, 0, (2*zFar*zNear)/(zNear-zFar), 0
    };

    glMultMatrixf(m);
}

RenderSystem :: RenderSystem() {
    this->prior = 3;
    this->start();
}
void RenderSystem :: start(){
    
}
void RenderSystem :: update(EntityManager& em, EventBus& ev, SDL_Window* window)  {
    glEnable(GL_DEPTH_TEST);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//颜色缓冲区（Framebuffer）清空

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (float)w / h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //把当前矩阵重置为“单位矩阵”
    //glTranslatef(0, 0, -15);//把整个世界移动 便于观察
    drawCrosshair(w, h, 10.0f, 2.0f); // draw crossing

    Entity camEntity = 0;
    Position* camPos = nullptr;
    Camera* cam = nullptr;
    for (auto e : em.view<Position, Camera>()) {
        camPos = em.get<Position>(e);
        cam = em.get<Camera>(e);
        camEntity = e;
        break; // 只取一个camera
    }
    if (!cam || !camPos) return;

    // 用 yaw / pitch 计算方向
    float yawRad   = cam->yaw   * M_PI / 180.f;
    float pitchRad = cam->pitch * M_PI / 180.f;

    Vec3 front{
        cosf(pitchRad) * sinf(yawRad),
        sinf(pitchRad),
        -cosf(pitchRad) * cosf(yawRad)
    };
    front = normalize(front);
    Vec3 right = normalize(cross(front, Vec3{0,1,0}));
    Vec3 up    = cross(right, front);
    Vec3 target = camPos->position + front;
    // 视锥剔除参数（简化版）
    float fov = 70.0f * M_PI / 180.0f; // 垂直视角弧度
    float aspect = (float)w / h;
    float nearPlane = 0.1f;
    float farPlane  = 100.0f;
    float tanHalfFOV = tanf(fov / 2.0f);
    //OpenGL 是 右乘矩阵，实际执行顺序是反过来的：先 yaw再 pitch 再 translate!!
    glRotatef(-cam->pitch, 1, 0, 0); // 绕局部 X
    glRotatef(-cam->yaw,   0, 1, 0); // 绕世界 Y
    glTranslatef(-camPos->position.x, -camPos->position.y, -camPos->position.z);   
    
    for (Entity e : em.view<Position>()) {
        //if (e == camEntity) continue; // 剔除摄像机自身
        if (camPos && em.get<Position>(e) == camPos) continue; //玩家的位置不能当作方块渲染
        auto* pos = em.get<Position>(e);
        auto* s = em.get<Size>(e);
        auto* coll = em.get<Collider>(e);
        if (!pos || !s || !coll) continue;
        // 构造方块 AABB
        // AABB box = coll->box; // 如果 coll->box 是世界坐标就直接用
        // if (!isAABBVisible(camPos->position, front, up, 70.f, 
        //     (float)w/h, 0.1f, 100.f, box))
        //     continue; // 剔除不可见
        drawCube(pos->position.x,pos->position.y,pos->position.z,s->value.x);
    }
    SDL_GL_SwapWindow(window);
}
// 🔥 真正的摄像机
// gluLookAt(
//     camPos->position.x, camPos->position.y, camPos->position.z,
//     target.x,           target.y,           target.z,
//     up.x,               up.y,               up.z
// );

bool RenderSystem::isAABBVisible(const Vec3& camPos, const Vec3& camFront, const Vec3& camUp,
                   float fov, float aspect, float nearDist, float farDist, const AABB& box) {
    // 摄像机右向量
    Vec3 camRight = normalize(cross(camFront, camUp));
    Vec3 up = normalize(camUp);

    // 近平面和远平面中心
    Vec3 nc = camPos + camFront * nearDist;
    Vec3 fc = camPos + camFront * farDist;

    float tanFov = tanf(fov * 0.5f * M_PI/180.f);
    float nh = nearDist * tanFov;
    float nw = nh * aspect;
    float fh = farDist * tanFov;
    float fw = fh * aspect;

    // 检查 AABB 每个角点是否至少在近平面和远平面之间
    bool inside = false;
    for (int xi=0; xi<=1; xi++){
        for (int yi=0; yi<=1; yi++){
            for (int zi=0; zi<=1; zi++){
                Vec3 corner = {
                    xi ? box.max.x : box.min.x,
                    yi ? box.max.y : box.min.y,
                    zi ? box.max.z : box.min.z
                };

                Vec3 local = corner - camPos;
                float zDist = dot(local, camFront);  // 相机前方向距离
                if (zDist < nearDist || zDist > farDist) continue;

                float yDist = dot(local, up);
                float xDist = dot(local, camRight);
                float yLimit = zDist * tanFov;
                float xLimit = yLimit * aspect;

                if (fabsf(xDist) <= xLimit && fabsf(yDist) <= yLimit) {
                    inside = true; // 角点在视锥内
                    break;
                }
            }
        }
    }
    return inside;
}
RenderSystem::~RenderSystem(){
    
}
