#pragma once

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable
{
public:
    bvh_node(const hittable_list &list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(const vector<shared_ptr<hittable>> &src_objects, size_t start, size_t end)
    // original method
    {
        auto objects = src_objects;

        int axis = random_int(0, 2);
        auto comparator = (axis == 0)   ? box_x_compare
                          : (axis == 1) ? box_y_compare
                                        : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1)
        {
            left = right = objects[start];
        }
        else if (object_span == 2)
        {
            if (comparator(objects[start], objects[start + 1]))
            {
                left = objects[start];
                right = objects[start + 1];
            }
            else
            {
                right = objects[start];
                left = objects[start + 1];
            }
        }
        else
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + object_span / 2;
            // no need to sort, just cut the vec by half will do the trick
            // but std::sort() is still slightly faster than our method :(
            // kth_partition(objects, axis, start, end - 1, mid);

            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        if (!bbox.hit(r, ray_t))
            return false;

        // when loop into leaf node, right/left should be primitive
        bool hit_l = left->hit(r, ray_t, hit);
        bool hit_r = right->hit(r, interval(ray_t.min, /*if left hit, then check any hit before left hit*/ hit_l ? hit.t : ray_t.max), hit);

        return hit_l || hit_r;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index)
    {
        return a->bounding_box().axis(axis_index).min < b->bounding_box().axis(axis_index).min;
    }

    static bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
    {
        return box_compare(a, b, 2);
    }

    static void kth_partition(vector<shared_ptr<hittable>> &objects, int axis, size_t start, size_t end, size_t k)
    {
        shared_ptr<hittable> key = objects[k];

        size_t i = start;
        size_t j = end;

        while (i < j)
        {
            while (box_compare(objects[j], key, axis) && i < j)
                --j;
            while (!box_compare(objects[i], key, axis) && i < j)
                ++i;

            std::swap(objects[i], objects[j]);
        }

        if (i == k)
            return;
        if (i > k)
            kth_partition(objects, axis, start, i - 1, k);
        if (i < k)
            kth_partition(objects, axis, i + 1, end, k);
    }
};