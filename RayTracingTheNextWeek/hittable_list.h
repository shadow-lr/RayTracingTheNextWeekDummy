#pragma once
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

// hitable_list是所有能够被光线撞击的物体的列表/集合
class hittable_list : public hittable {
public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
    std::vector<shared_ptr<hittable>> objects;
};

// 遍历objects中所有对象，与当前的射线进行相交检测
bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }


/*依次判断列表中所有物体是否被光线撞到，每次判断一个。若有被撞到，则将撞点信息保存在hit_record结构体中。我们可以看到rec是可能被写多次的，最终保存的值是后一次的值，也就是真正有效的值是后一次的值，也就是离观测点最近的物体的有效撞点（“有效撞点”：对于单个物体，会筛选出一个局部有效撞点；对于多个物体，从所有单个物体各自的局部有效撞点筛选出最终一个整体有效撞点）。因为不管这条光线依次撞击了多少个物体产生多少个撞点，我们能看到的只是离我们最近的撞点*/
/*如果当前撞点在范围内，则将当前撞点的距离设置为范围的最大值。也就是后面只考虑比该撞点更近的撞点。趋势是：找到的撞点是越来越近的，最终找到最近的撞点。*/
    return hit_anything;
}

#endif