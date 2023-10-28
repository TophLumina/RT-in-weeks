#pragma once

#include "rtweekend.h"

#include "hittable.h"

class quad : public hittable
{
public:
    quad(const point3 &_Q, const vec3 &_u, const vec3 &_v, shared_ptr<material> m) : Q(_Q), u(_u), v(_v), mat(m)
    {
        auto n = cross(u, v);
        normal = normalize(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        bbox = aabb(Q, Q + u + v).pad();
    }

    // Given the hit point in plane_UV coordinates, return false if outside the primitive, otherwise fill the hit_info with UV coords and return true
    virtual bool is_interior(double a, double b, hit_info &hit) const
    {
        if (a < 0 || b < 0 || a > 1 || b > 1)
            return false;

        hit.u = a;
        hit.v = b;
        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        auto denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane
        if (fabs(denom) < 1e-8)
            return false;

        // illegal hit if t is outside the ray interval
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Check if the hit point lies inside the quad
        auto p = r.at(t);
        vec3 p_vec = p - Q;
        auto alpha = dot(w, cross(p_vec, v));
        auto beta = dot(w, cross(u, p_vec));
        
        if (!is_interior(alpha, beta, hit))
            return false;

        // Hit confirm, fill the hit_info
        hit.t = t;
        hit.hit_point = p;
        hit.mat = mat;
        hit.set_face_normal(r, normal);

        return true;
    }

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w; // Cached value use for solving UV
};