#pragma once

#include "rtweekend.h"
#include "aabb.h"

class material;

class hit_info
{
public:
    point3 hit_point;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    double u;
    double v;
    bool front_face;

    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        // Assume that all normals have been normalized

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_info &hit) const = 0;
    virtual aabb bounding_box() const = 0;
};

class translate : public hittable
{
public:
    translate(shared_ptr<hittable> p, const vec3 &_transform) : object(p), offset(_transform)
    {
        bbox = p->bounding_box() + offset;
    }

    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        // Move ray backward by the offset
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Check for any hit point alone the offset ray
        if (!object->hit(offset_r, ray_t, hit))
            return false;

        // Now move the hit point forward by the offset
        hit.hit_point += offset;
        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};