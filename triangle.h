#pragma once

#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"

class triangle : public hittable
{
public:
    triangle(const point3 &_Q, const vec3 &_u, const vec3 &_v, shared_ptr<material> m) : Q(_Q), u(_u), v(_v), mat(m)
    {
        auto n = cross(u, v);
        normal = normalize(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        area = length(n) / 2;

        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        bbox = aabb(Q, Q + u + v).pad();
    }

    // Given the hit point in plane_UV coordinates, return false if outside the primitive, otherwise fill the hit_info with UV coords and return true
    virtual bool is_interior(double a, double b, hit_info &hit) const
    {
        if (a < 0 || b < 0 || a > 1 || b > 1 || a + b > 1 || a + b < 0)
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

        // Check if the hit point lies inside the triangle
        auto p = r.at(t);
        vec3 p_vec = p - Q;
        auto alpha = dot(w, cross(p_vec, v));
        auto beta = dot(w, cross(u, p_vec));

        if (!is_interior(alpha, beta, hit))
            return false;

        // Hit confirm, fill the hit_info
        hit.hittable_index = index;
        hit.t = t;
        hit.hit_point = p;
        hit.mat = mat;
        hit.set_face_normal(r, normal);

        return true;
    }

    double pdf_value(const point3 &origin, const vec3 &v) const override
    {
        hit_info hit;
        if (!this->hit(ray(origin, v), interval(0, infinity), hit))
            return 0;

        auto distance_squared = hit.t * hit.t * squared_length(v);
        auto cosine = fabs(dot(v, hit.normal) / length(v));

        return distance_squared / (cosine * area);
    }

    vec3 random(const point3 &origin) const override
    {
        auto a = random_double();
        auto b = random_double();
        auto p = Q + b * (a * u + (1 - a) * v);
        return p - origin;
    }

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
    double area;
};
