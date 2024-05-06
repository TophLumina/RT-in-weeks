#pragma once

#include "Math/utility/MatrixOperations.hpp"
#include "Math/utility/MatrixTransforms.hpp"
#include "hittable.h"
#include "hittable_list.h"
#include "vec.h"

#include <array>

class quad : public hittable
{
public:
    quad(const point3 &a, const point3 &b, const point3 &c, const point3 &d, shared_ptr<material> m) : vertices({a, b, c, d}), mat(m)
    {
        Q = vertices[0];
        u = vertices[1] - vertices[0];
        v = vertices[3] - vertices[0];

        auto n = cross(u, v);
        normal = normalize(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        area = length(n);

        geometric_center = Q + 0.5 * u + 0.5 * v;
        update_bounding_box();
    }

    quad(const point3 &_Q, const vec3 &_u, const vec3 &_v, shared_ptr<material> m) : Q(_Q), u(_u), v(_v), mat(m)
    {
        vertices = {Q, Q + u, Q + u + v, Q + v};

        auto n = cross(u, v);
        normal = normalize(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        area = length(n);

        geometric_center = Q + 0.5 * u + 0.5 * v;
        update_bounding_box();
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    void translate(const vec3 &offsets) override
    {
        auto m_translation = Math::Matrix::translate(offsets);
        *m_transform = Math::Matrix::translate(offsets) * *m_transform;
        Q += offsets;
        D = dot(normal, Q);

        geometric_center = Q + 0.5 * u + 0.5 * v;
        bbox = bbox + offsets;
    }

    void scale(const vec3 &scalar, const vec3 &center) override
    {
        
    }

    void rotate(const vec3 &axis, double angle, const vec3 &center) override
    {
        auto m_rotation = Math::Matrix::rotate(axis, angle, center);
        *m_transform = Math::Matrix::rotate(axis, angle, center) * *m_transform;
        Q = m_rotation * vec4(Q, 1.0);
        u = m_rotation * vec4(u, 0.0);
        v = m_rotation * vec4(v, 0.0);

        auto n = cross(u, v);
        normal = m_rotation * vec4(normal, 0.0);
        D = dot(normal, Q);
        w = n / dot(n, n);

        geometric_center = Q + 0.5 * u + 0.5 * v;
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

        // Check if the hit point lies inside the quad
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
        if (!this->hit(ray(origin, v), interval(0.001, infinity), hit))
            return 0;

        auto distance_squared = hit.t * hit.t * squared_length(v);
        auto cosine = fabs(dot(v, hit.normal) / length(v));

        return distance_squared / (cosine * area);
    }

    vec3 random(const point3 &origin) const override
    {
        auto p = Q + (random_double() * u) + (random_double() * v);
        return p - origin;
    }

private:
    std::array<point3, 4> vertices;
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w; // Cached value use for solving UV
    double area;

    void update_bounding_box()
    {
        bbox = aabb(Q, Q + u + v).pad();
    }

    // Given the hit point in plane_UV coordinates, return false if outside the primitive, otherwise fill the hit_info with UV coords and return true
    bool is_interior(double a, double b, hit_info &hit) const
    {
        if (a < 0 || b < 0 || a > 1 || b > 1)
            return false;

        hit.u = a;
        hit.v = b;
        return true;
    }
};

// Return a 3D cube that contains the two opposite vertices a & b
inline shared_ptr<hittable_list> cube(const point3 &a, const point3 &b, shared_ptr<material> mat)
{
    auto faces = make_shared<hittable_list>();

    // Construct the two opposite vertices
    auto min = point3(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
    auto max = point3(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));

    auto dx = vec3(max.x - min.x, 0, 0);
    auto dy = vec3(0, max.y - min.y, 0);
    auto dz = vec3(0, 0, max.z - min.z);

    faces->add(make_shared<quad>(point3(min.x, min.y, max.z), dx, dy, mat));  // front
    faces->add(make_shared<quad>(point3(max.x, min.y, max.z), -dz, dy, mat)); // right
    faces->add(make_shared<quad>(point3(max.x, min.y, min.z), -dx, dy, mat)); // back
    faces->add(make_shared<quad>(point3(min.x, min.y, min.z), dz, dy, mat));  // left
    faces->add(make_shared<quad>(point3(min.x, max.y, max.z), dx, -dz, mat)); // top
    faces->add(make_shared<quad>(point3(min.x, min.y, min.z), dx, dz, mat));  // bottom

    return faces;
}