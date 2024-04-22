#pragma once

#include <memory>
#include <vector>

#include "hittable.h"
#include "aabb.h"

using std::make_shared;
using std::shared_ptr;
using std::vector;

class hittable_list : public hittable
{
public:
    vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    aabb bounding_box() const override { return bbox; }

    void clear()
    {
        objects.clear();
    }

    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    // Avoid nested hittable_list in objects, otherwise it will lead to an incomplete build of BVH Tree
    // Why there is no difference on test run? heavily nested structure should have caused serious performance issues...
    void add(shared_ptr<hittable_list> object_list)
    {
        for (auto i : object_list->objects)
            objects.push_back(i);

        bbox = aabb(bbox, object_list->bounding_box());
    }

    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        hit_info tmp;
        bool hit_any = false;
        auto closest_so_far = ray_t.max;

        for (const auto &object : objects)
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), tmp))
            {
                hit_any = true;
                closest_so_far = tmp.t;
                hit = tmp;
            }
        }

        return hit_any;
    }

    double pdf_value(const point3 &origin, const vec3 &v) const override
    {
        if (!objects.empty())
        {
            auto sum = 0.0;

            for (const auto &object : objects)
                sum += object->pdf_value(origin, v);

            return sum / objects.size();
        }

        return hittable::pdf_value(origin, v);
    }

    vec3 random(const vec3 &origin) const override
    {
        if (!objects.empty())
        {
            int index = random_int(0, objects.size() - 1);
            return objects[index]->random(origin);
        }

        return hittable::random(origin);
    }

    // for shadow rays only
    shared_ptr<hittable> operator[](int i) const
    {
        return objects[i];
    }

    shared_ptr<hittable> random_hittable() const
    {
        if (objects.empty())
            return nullptr;

        return objects[random_int(0, objects.size() - 1)];
    }

private:
    aabb bbox;
};