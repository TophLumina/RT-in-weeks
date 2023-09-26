#pragma once

#include "rtweekend.h"

class material;

class hit_info
{
public:
    point3 hit_point;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
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
};