#pragma once

#include <memory>
#include <vector>

#include "aabb.h"
#include "hittable.h"

using std::make_shared;
using std::shared_ptr;
using std::vector;

class hittable_list : public hittable
{
public:
    vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> hittable) { add(hittable); }

    aabb bounding_box() const override { return bbox; }

    void translate(const vec3 &offset) override
    {
        *m_transform = Math::Matrix::translate(offset) * *m_transform;
        for (auto &object : objects)
            object->translate(offset);

        geometric_center = Math::Matrix::transform(*m_transform, vec4(geometric_center, 1.0));
        update_bounding_box();
    }

    void rotate(const vec3 &axis, double angle, const vec3 &center) override
    {
        *m_transform = Math::Matrix::rotate(axis, angle, center) * *m_transform;
        for (auto &object : objects)
            object->rotate(axis, angle, center);

        geometric_center = Math::Matrix::transform(*m_transform, vec4(geometric_center, 1.0));
        update_bounding_box();
    }

    void rotate(const vec3 &axis, double angle) override
    {
        rotate(axis, angle, geometric_center);
    }

    void clear()
    {
        objects.clear();
    }

    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);

        geometric_center = (geometric_center * (objects.size() - 1) + object->geometric_center) / objects.size();
        bbox = aabb(bbox, object->bounding_box());
    }

    // Maybe it's better to keep a nested structure in hittable_list to maintain the parent-child relationship
    // Or use nested hittable_list in geometry, but not in BVH Tree construction
    void add(shared_ptr<hittable_list> object_list)
    {
        objects.push_back(object_list);

        geometric_center = (geometric_center * (objects.size() - object_list->objects.size()) + object_list->geometric_center * object_list->objects.size()) / objects.size();
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

    // FIXME: Bugs in hittable_list flatten method
    // Flatten the nested hittable_list for full BVH Tree construction
    shared_ptr<hittable_list> flattened_for_bvh() const
    {
        auto list = make_shared<hittable_list>();
        list->flatten_list(make_shared<hittable_list>(*this));

        return list;
    }

private:
    aabb bbox;

    void update_bounding_box()
    {
        bbox = aabb();
        for (const auto &object : objects)
            bbox = aabb(bbox, object->bounding_box());
    }

    // FIXME: Bugs in hittable_list flatten method
    // Flatten the nested hittable_list for full BVH Tree construction
    void flatten_list(shared_ptr<hittable_list> src_list)
    {
        for (const auto &object : src_list->objects)
        {
            auto nested_list = std::dynamic_pointer_cast<hittable_list>(object);
            if (nested_list)
                flatten_list(nested_list);
            else
                add(object);
        }

        geometric_center = (geometric_center * (objects.size() - src_list->objects.size()) + src_list->geometric_center * src_list->objects.size()) / objects.size();
        bbox = aabb(bbox, src_list->bounding_box());
    }
};