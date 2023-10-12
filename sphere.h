#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
    sphere(point3 _center1, double _radius, shared_ptr<material> _material) : center0(_center1), radius(_radius), mat(_material), is_moving(false), center_vec(vec3())
    {
        vec3 rvec = vec3(radius, radius, radius);
        bbox = aabb(_center1 + rvec, _center1 - rvec);
    }

    sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material) : center0(_center1), radius(_radius), mat(_material), is_moving(_center1 == _center2 ? false : true), center_vec(_center1 == _center2 ? vec3() : _center2 - _center1)
    {
        vec3 rvec = vec3(radius, radius, radius);
        if (is_moving)
        {
            aabb box0 = aabb(_center1 + rvec, _center1 - rvec);
            aabb box1 = aabb(_center2 + rvec, _center2 - rvec);
            bbox = aabb(box0, box1);
        }
        else
        {
            bbox = aabb(center0 + rvec, center0 - rvec);
        }
    }

    // According to ray info load the hit_info if hit
    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        point3 center = is_moving ? sphere::center(r.time()) : center0;
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
        hit.mat = mat;

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    point3 center0;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;
    vec3 center_vec;
    aabb bbox;

    point3 center(double time) const
    {
        return is_moving ? center0 + time * center_vec : center0;
    }
};