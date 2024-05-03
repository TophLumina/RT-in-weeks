#pragma once

#include "Math/utility/MatrixOperations.hpp"
#include "Math/utility/MatrixTransforms.hpp"
#include "ONB.h"
#include "hittable.h"
#include "vec.h"


class sphere : public hittable
{
public:
    sphere(point3 _origin, double _radius, shared_ptr<material> _material) : origin(_origin), radius(_radius), mat(_material)
    {
        center = origin;

        geometric_center = center;
        update_bounding_box();
    }

    // According to ray info load the hit_info if hit
    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        point3 center = this->center;
        vec3 oc = r.origin() - center;
        auto a = squared_length(r.direction());
        auto half_b = dot(r.direction(), oc);
        auto c = squared_length(oc) - radius * radius;

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

        // Hit confirm, fill the hit_info
        hit.hittable_index = index;
        hit.t = root;
        hit.hit_point = r.at(hit.t);
        vec3 outward_normal = (hit.hit_point - center) / radius;
        hit.set_face_normal(r, outward_normal);
        get_uv(outward_normal, hit.u, hit.v);
        hit.mat = mat;

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    void translate(const vec3 &offsets) override
    {
        *m_transform = Math::Matrix::translate(*m_transform, offsets);
        center = Math::Matrix::transform(*m_transform, vec4(origin, 1.0));

        geometric_center = center;
        update_bounding_box();
    }

    // no scaling for sphere

    void rotate(const vec3 &axis, double angle, const vec3 &center) override
    {
        *m_transform = Math::Matrix::rotate(*m_transform, axis, angle, center);
        this->center = Math::Matrix::transform(*m_transform, vec4(center, 1.0));

        geometric_center = this->center;
        update_bounding_box();
    }

    void rotate(const vec3 &axis, double angle) override
    {
        rotate(axis, angle, this->origin);
    }

    void parent_transform(const mat4 &transform) override
    {
        *m_transform = (*m_transform) * transform;
        center = Math::Matrix::transform(*m_transform, vec4(origin, 1.0));

        geometric_center = center;
        update_bounding_box();
    }

    double pdf_value(const point3 &_origin, const vec3 &v) const override
    {
        hit_info hit;
        if (!this->hit(ray(_origin, v), interval(0.001, infinity), hit))
            return 0;

        auto cos_theta_max = sqrt(1 - pow(radius, 2) / squared_length(center - _origin));
        auto solid_angle = 2 * Math::M_PI * (1 - cos_theta_max);

        return 1 / solid_angle;
    }

    vec3 random(const point3 &_origin) const override
    {
        vec3 dir = center - _origin;
        auto dist_squared = squared_length(dir);
        onb coord;
        coord.build_from_w(dir);
        return coord.local(random2sphere(radius, dist_squared));
    }

private:
    point3 origin;
    point3 center;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;

    void update_bounding_box()
    {
        vec3 rvec = vec3(radius, radius, radius);
        bbox = aabb(center - rvec, center + rvec).pad();
    }

    static void get_uv(const point3 &p, double &u, double &v)
    {
        // p: a given point on the sphere of radius one, centered at the origin
        // u: returned value [0,1] of angle around the Y axis from X=-1
        // v: returned value [0,1] of angle from Y=-1 to Y=+1
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y);
        auto phi = atan2(-p.z, p.x) + Math::M_PI;

        u = phi / (2 * Math::M_PI);
        v = theta / Math::M_PI;
    }

    static vec3 random2sphere(double radius, double dist_squared)
    {
        auto r1 = random_double();
        auto r2 = random_double();
        auto z = 1 + r2 * (sqrt(1 - pow(radius, 2) / dist_squared) - 1);

        auto phi = 2 * Math::M_PI * r1;
        auto x = cos(phi) * sqrt(1 - pow(z, 2));
        auto y = sin(phi) * sqrt(1 - pow(z, 2));

        return vec3(x, y, z);
    }
};