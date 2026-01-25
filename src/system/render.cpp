#include "../system/system.h"
#include "../component/component.h"
#include "../utils/math.h"
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
struct PosColorVertex {
        float x, y, z;
        uint32_t abgr;
        static bgfx::VertexLayout ms_decl;
    };
bgfx::VertexLayout PosColorVertex::ms_decl;

void RenderSystem :: drawCrosshair(int screenWidth, int screenHeight, float size, float thickness) {
    float cx = screenWidth/2.0f;
    float cy = screenHeight/2.0f;
    PosColorVertex crosshair[4] = {
        {cx-size, cy, 0.0f, 0xffffffff},
        {cx+size, cy, 0.0f, 0xffffffff},
        {cx, cy-size, 0.0f, 0xffffffff},
        {cx, cy+size, 0.0f, 0xffffffff}
    };
    float ortho[16];
    bx::mtxOrtho(ortho, 0.0f, float(screenWidth), 0.0f, float(screenHeight), -1.0f, 1.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setTransform(ortho);

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(crosshair, sizeof(crosshair)),
        PosColorVertex::ms_decl
    );
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_PT_LINES); // 线条模式
    bgfx::submit(1, BGFX_INVALID_HANDLE);
}

void RenderSystem :: drawCube(float x, float y, float z, float s, const float viewProj[16]) {
    // --- 顶点 ---
    PosColorVertex cubeVertices[8] = {
        {x,       y,       z,       0xff33cc33},
        {x+s,     y,       z,       0xff33cc33},
        {x+s,     y+s,     z,       0xff33cc33},
        {x,       y+s,     z,       0xff33cc33},
        {x,       y,       z+s,     0xff33cc33},
        {x+s,     y,       z+s,     0xff33cc33},
        {x+s,     y+s,     z+s,     0xff33cc33},
        {x,       y+s,     z+s,     0xff33cc33},
    };

    uint16_t cubeIndices[36] = {
        0,1,2, 0,2,3,
        4,5,6, 4,6,7,
        0,4,7, 0,7,3,
        1,5,6, 1,6,2,
        3,2,6, 3,6,7,
        0,1,5, 0,5,4
    };

    // --- bgfx 缓冲 ---
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(cubeVertices, sizeof(cubeVertices)),
        PosColorVertex::ms_decl
    );
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(cubeIndices, sizeof(cubeIndices))
    );

    // 模型矩阵（单位矩阵，因为顶点已在世界坐标）
    float model[16];
    bx::mtxIdentity(model);

    float mvp[16];
    bx::mtxMul(mvp, viewProj, model);

    bgfx::setTransform(mvp);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);
    bgfx::setState(BGFX_STATE_DEFAULT); // 默认状态即可
    bgfx::submit(0, BGFX_INVALID_HANDLE);
}

RenderSystem :: RenderSystem() {
    this->prior = 3;
    this->start();
}
void RenderSystem :: start(){
    PosColorVertex::ms_decl
    .begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
    .end();
}
void RenderSystem :: update(EntityManager& em, EventBus& ev, SDL_Window* window)  {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    bgfx::reset(800, 600, BGFX_RESET_VSYNC);
    bgfx::setViewClear( // 清屏
        0,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x87CEEBFF, // 天空蓝
        1.0f,
        0
    );
    bgfx::setViewRect(0, 0, 0, 800, 600);
    
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
    bx::Vec3 front{
        cosf(pitchRad) * sinf(yawRad),
        sinf(pitchRad),
        -cosf(pitchRad) * cosf(yawRad)
    };
    front = normalize(front);
    bx::Vec3 right = normalize(cross(front, bx::Vec3{0,1,0}));
    bx::Vec3 up    = cross(right, front);
    //bx::Vec3 target = camPos->position + front;
    // 视锥剔除参数（简化版）
    float fov = 70.0f * M_PI / 180.0f; // 垂直视角弧度
    float aspect = (float)w / h;
    float nearPlane = 0.1f;
    float farPlane  = 100.0f;
    float tanHalfFOV = tanf(fov / 2.0f);
    // View 矩阵
    float view[16];
    bx::Vec3 eye(camPos->position.x, camPos->position.y, camPos->position.z);
    bx::Vec3 center(camPos->position.x + front.x, camPos->position.y + front.y, camPos->position.z + front.z);
    bx::Vec3 upVec(up.x, up.y, up.z);
    bx::mtxLookAt(view, eye, center, upVec); // 这里只支持bx::Vec3

    // Projection 矩阵
    float proj[16];
    bx::mtxProj(proj, 70.0f, float(w)/float(h), 0.1f, 100.0f,
                bgfx::getCaps()->homogeneousDepth);
    float viewProj[16];
    bx::mtxMul(viewProj, proj, view); 
    drawCrosshair(w, h, 10.0f, 2.0f);

    for (Entity e : em.view<Position>()) {
        //if (e == camEntity) continue; // 剔除摄像机自身
        if (camPos && em.get<Position>(e) == camPos) continue; //玩家的位置不能当作方块渲染
        auto* pos = em.get<Position>(e);
        auto* s = em.get<Size>(e);
        auto* coll = em.get<Collider>(e);
        if (!pos || !s || !coll) continue;
        drawCube(pos->position.x, pos->position.y, pos->position.z, s->value.x, viewProj);
        
        // 构造方块 AABB
        // AABB box = coll->box; // 如果 coll->box 是世界坐标就直接用
        // if (!isAABBVisible(camPos->position, front, up, 70.f, 
        //     (float)w/h, 0.1f, 100.f, box))
        //     continue; // 剔除不可见
    }
    // 在 bgfx 里，每个渲染操作都绑定到一个 view，viewId = 0 通常是默认主视图
    //bgfx::touch(0); 
    // / 告诉 bgfx：“这个视图（view）这帧依然有效”，“即使我没往里面提交任何 draw call，
    // 也要清理/执行这个 view 的状态（clear, stencil, etc.）
    // 如果你每帧都提交 draw call，通常不必 touch。touch 主要用于空 view 也要清理的情况
    bgfx::frame();
}


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
