#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"
#include "texture.h"

struct hit_record;

// 告诉射线如何与表面相互作用
class material {
public:
    /**
     * @brief 函数简要说明-测试函数
     * @param r_in              参数1 射线
     * @param rec               参数2 离光线起点的距离t、撞点的坐标向量p、撞点出的法向量normal.
     * @param attenuation       参数3 衰弱后的颜色值
     * @param scattered         参数4 散射射线
     *
     * @return 返回说明
     */
    virtual bool scatter(
            const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered
    ) const = 0;

    virtual ~material() {}
};

// 兰伯特模型类
class lambertian : public material {
public:
    lambertian(const color &a) : albedo(make_shared<solid_color>(a)) {}

    lambertian(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(
            const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered
    ) const override {
        // 散射的向量
        vec3 scatter_direction = rec.normal + random_unit_vector();
//        vec3 scatter_direction = rec.normal + random_in_unit_sphere();
//        vec3 scatter_direction = rec.normal + random_in_hemisphere(rec.normal);

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

// 金属类
class metal : public material {
public:
    metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
//        scattered = ray(rec.p, reflected);
// 扰动 fuzz * random_unit_sphere
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz;
};

// dielectric电介质类
class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        // 先根据法线和入射向量计算出角度的三角函数值 然后再根据公式 n1 / n2 * sin(θ) > 1 大于1就全反射，反之 散射
        vec3 unit_direction = unit_vector(r_in.direction());
        vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

        // new content
        // dot(v, n) = cos(θ)
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        vec3 direction;

        // 真正的玻璃具有与角度不同的反射率————从陡峭的角度看窗户，它就会变成一面镜子
        if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_double()) {
            // Must Reflect 必定反射 直接执行反射公式
            direction = reflect(unit_direction, rec.normal);
        } else {
            // Can refract 可以散射 走折射公式
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

public:
    // 折射率
    double ir;  // Index Of Refraction
private:
    // Christophe Schlick
    // 使用 Schlick 近似计算反射率
    static double reflectance(double cosine, double ref_idx) {
        double r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif