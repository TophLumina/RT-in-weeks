#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
    sphere(point3 _center, double _radius) : center(_center), radius(_radius) {}

    // According to ray info load the hit_info if hit
    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = sqrt(discriminant);

        // Nearest root within ray_t(t_min -> t_max)
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        hit.t = root;
        hit.hit_point = r.at(hit.t);
        vec3 outward_normal = (hit.hit_point - center) / radius;
        hit.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 center;
    double radius;
};