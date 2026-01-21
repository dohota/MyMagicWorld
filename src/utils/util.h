#include "../component/component.h"



// 返回 t 值（距离），如果不碰撞返回 -1
inline float rayAABBIntersect(const Ray& ray, const AABB& box) {
    float tmin = (box.min.x - ray.origin.x) / ray.dir.x;
    float tmax = (box.max.x - ray.origin.x) / ray.dir.x;
    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (box.min.y - ray.origin.y) / ray.dir.y;
    float tymax = (box.max.y - ray.origin.y) / ray.dir.y;
    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax)) return -1;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (box.min.z - ray.origin.z) / ray.dir.z;
    float tzmax = (box.max.z - ray.origin.z) / ray.dir.z;
    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax)) return -1;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmin >= 0 ? tmin : tmax; // 返回最近的正交点
}
