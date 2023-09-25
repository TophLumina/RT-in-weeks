#pragma once

#include <memory>
#include <vector>

#include "hittable.h"

using std::make_shared;
using std::shared_ptr;
using std::vector;

class hittable_list : public hittable
{
public:
    vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object){ add(object); }

    void clear()
    {
        objects.clear();
    }

    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);
    }

    bool hit(const ray &r, interval ray_t, hit_info &info) const override
    {
        hit_info tmp;
        bool hit_any = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects)
        {
            if (object -> hit(r, interval(ray_t.min, closest_so_far), tmp))
            {
                hit_any = true;
                closest_so_far = tmp.t;
                info = tmp;
            }
        }

        return hit_any;
    }
};