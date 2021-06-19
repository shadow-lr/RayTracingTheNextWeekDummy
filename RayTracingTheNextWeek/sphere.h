#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere() {}

    sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};

// 另一种写法
/*
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        vec3 oc = r.orgin() - center;
        float a = oc.dot(r.direction(), r.direction());
        float b = 2.0 * oc.dot(oc, r.direction());
        float c = oc.dot(oc, oc) - radius*radius;
        float discriminant = b*b - 4*a*c;

        if (discriminant > 0) {
            float temp = (-b - sqrt(discriminant)) / (2.0*a);
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.point_at_parameter(rec.t);
                rec.normal = (rec.p - center) / radius;
                return true;
            }
            temp = (-b + sqrt(discriminant)) / (2.0*a);
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.point_at_parameter(rec.t);
                rec.normal = (rec.p - center) / radius;
                return true;
            }
        }
//判断小根和大根是否在范围内。首先判断小根是否在范围内。是：保存相关信息，然后直接返回；否，判断大根是否在范围内。也就是优先选小根，小根不行再考虑大根
return false;
}
 *
 * */
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