#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb {
public:
    aabb() {}

    aabb(const point3 &a, const point3 &b) : minimum(a), maximum(b) {}

    point3 min() const { return minimum; }

    point3 max() const { return maximum; }

    bool hit(const ray &r, double t_min, double t_max) const;

public:
    point3 minimum;
    point3 maximum;
};

bool aabb::hit(const ray &r, double t_min, double t_max) const {
    for (int a = 0; a < 3; ++a) {
        // X Y Z 一起比较 不断更新 t_min 和 t_max 取最小的t_min 和 最大的t_max
//            auto t0 = fmin((minimum[a] - r.origin()[a]) / r.direction()[a],
//                           (maximum[a] - r.origin()[a]) / r.direction()[a]);
//
//            auto t1 = fmax((minimum[a] - r.origin()[a]) / r.direction()[a],
//                           (maximum[a] - r.origin()[a]) / r.direction()[a]);

        // 优化
        auto invD = 1.0f / r.direction()[a];

        auto t0 = (min()[a] - r.origin()[a]) * invD;
        auto t1 = (max()[a] - r.origin()[a]) * invD;

        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
            return false;
    }
    return true;
}

// 将两个aabb重组成一个aabb
aabb surrounding_box(const aabb &box0, const aabb &box1) {
    point3 small(fmin(box0.min().x(), box1.min().x()),
                 fmin(box0.min().y(), box1.min().y()),
                 fmin(box0.min().z(), box1.min().z()));

    point3 big(fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));

    return aabb(small, big);
}

#endif //AABB_H
