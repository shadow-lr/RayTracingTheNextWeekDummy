#pragma once
#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "rtweekend.h"
#include "aabb.h"

class material;

/*该结构体记录“撞点”处的信息：离光线起点的距离t、撞点的坐标向量p、撞点出的法向量normal.*/
struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    // 物体命中点的U,V表面坐标
    double u, v;
    bool front_face;

    // 如果射线和法线的方向相同，则该射线在对象内部，如果射线和法线的方向相反，则该射线在对象之外

    // 该决定取决于您是要在几何相交时还是在着色时确定表面的侧面，如果反射与入射反向front_face = true
    inline void set_face_normal(const ray &r, const vec3 &outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

//hitable这个类表示能够被光线撞上的任何物体。比如，球体
class hittable {
public:
    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const = 0;

    virtual bool bounding_box(double time0, double time1, aabb &output_box) const = 0;
};

#endif