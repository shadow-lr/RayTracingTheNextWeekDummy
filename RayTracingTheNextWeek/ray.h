//#pragma once
//#ifndef RAY_H
//#define RAY_H
//
//#include "vec3.h"
//
//class ray {
//public:
//    ray() {}
//
//    ray(const point3 &origin, const vec3 &direction)
//            : orig(origin), dir(direction) {}
//
//    point3 origin() const { return orig; }
//
//    vec3 direction() const { return dir; }
//
//    point3 at(double t) const {
//        return orig + t * dir;
//    }
//
//public:
//    // A + tb
//    // A
//    point3 orig{};
//    // d
//    vec3 dir{};
//};
//
//#endif

#pragma once
#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
            : orig(origin), dir(direction)
    {}

    point3 origin() const  { return orig; }
    vec3 direction() const { return dir; }

    point3 at(double t) const {
        return orig + t*dir;
    }

public:
    point3 orig;
    vec3 dir;
};

#endif