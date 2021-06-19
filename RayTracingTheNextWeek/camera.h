#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
public:
    // vfov : vertical field-of-view in degrees

    // focus_dist : 焦距
    // aperture : 光圈直径
    camera(point3 lookfrom, point3 lookat, vec3 vup,
           double vfov, double aspect_ratio, double aperture,
           double focus_dist) {
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2);
        double viewport_height = 2.0 * h;
        double viewport_width = aspect_ratio * viewport_height;
        double focal_length = 1.0;

        auto w = unit_vector(lookfrom - lookat);
        auto u = unit_vector(cross(vup, w));
        auto v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;   // new version
        vertical = focus_dist * viewport_height * v;    // new version
        // lower_left_corner = origin - horizontal/2 - vertical/2 - w;
        // origin = lookfrom,   origin - w ==>> lookfrom - unit_vector(lookfrom - lookat)
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;    // new version
        lens_radius = aperture / 2;
    }

//    ray get_ray(double s, double t) const {
//        return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
//    }

    ray get_ray(double s, double t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();

        return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    vec3 u, v, w;
    double lens_radius;
};

#endif