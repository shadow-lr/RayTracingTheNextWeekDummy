#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"

class sphere : public hittable {
public:
    sphere() {}

    sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

    // 正方体AABB包围盒 包围 球
    virtual bool bounding_box(double time0, double time1, aabb &output_box) const {
        output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
        return true;
    }

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};


// sphere与光线求交判定
bool sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    // 求交
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;

    // 没根的情况下返回
    if (discriminant < 0)
        return false;
    double sqrtd = sqrt(discriminant);

    // 找到最近的root点在直线中可接受范围内
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    // 更新hit_record的rec信息
    rec.t = root;
    rec.p = r.at(rec.t);
    // 中心点到原上点的向量 再 除以 半径（向量的长度）
    rec.normal = (rec.p - center) / radius;

    rec.mat_ptr = mat_ptr;

    // 表面法线方向一定与入射相反的
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    return true;
}

#endif