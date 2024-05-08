#pragma once

#include "hittable.h"
#include <array>

class triangle : public hittable
{
public:
    triangle(const point3 &a, const point3 &b, const point3 &c, shared_ptr<material> m) : vertices({a, b, c}), mat(m)
    {
        Q = vertices[0];
        u = vertices[1] - vertices[0];
        v = vertices[2] - vertices[0];

        auto n = cross(u, v);
        normal = normalize(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        area = length(n) / 2;

        update_bounding_box();
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    void translate(const vec3 &offset) override
    {
        auto m_translation = Math::Matrix::translate(offset);
        *m_transform = m_translation * *m_transform;
        Q += offset;
        D = dot(normal, Q);

        geometric_center = Q + (u + v) / 3;
        bbox = bbox + offset;
    }

    void scale(const vec3 &scalar, const vec3 &center) override
    {
        auto m_scaling = Math::Matrix::scale(scalar, center);
        *m_transform = m_scaling * *m_transform;
        Q = m_scaling * vec4(Q, 1.0);
        u = m_scaling * vec4(u, 0.0);
        v = m_scaling * vec4(v, 0.0);

        auto n = cross(u, v);
        // The normal is scaled by the inverse of the transpose of the scaling matrix
        normal = inverse(transpose(m_scaling)) * vec4(normal, 0.0);
        D = dot(normal, Q);
        w = n / dot(n, n);
        area = length(n) / 2;

        geometric_center = Q + (u + v) / 3;
        update_bounding_box();
    }

    void scale(const vec3 &scalar) override
    {
        scale(scalar, geometric_center);
    }

    void rotate(const vec3 &axis, double angle, const vec3 &center) override
    {
        auto m_rotation = Math::Matrix::rotate(axis, angle, center);
        *m_transform = m_rotation * *m_transform;
        Q = m_rotation * vec4(Q, 1.0);
        u = m_rotation * vec4(u, 0.0);
        v = m_rotation * vec4(v, 0.0);

        auto n = cross(u, v);
        normal = m_rotation * vec4(normal, 0.0);
        D = dot(normal, Q);
        w = n / dot(n, n);

        geometric_center = Q + (u + v) / 3;
        update_bounding_box();
    }

    void rotate(const vec3 &axis, double angle) override
    {
        rotate(axis, angle, geometric_center);
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
    std::array<point3, 3> vertices;
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
    double area;

    virtual void update_bounding_box()
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
};
