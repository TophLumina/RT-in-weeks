#pragma once

#include "aabb.h"
#include "ray.h"
#include "rtweekend.h"


#include <memory>


class material;

class hit_info
{
public:
    point3 hit_point;
    vec3 normal;
    shared_ptr<material> mat;
    unsigned int hittable_index; // For debugging and denoising
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
    static unsigned int index;  // For debugging and denoising

    hittable() { ++index; }
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_info &hit) const = 0;
    virtual aabb bounding_box() const = 0;

    virtual double pdf_value(const point3 &origin, const vec3 &v) const { return 0.0; }
    virtual vec3 random(const vec3 &origin) const { return vec3(1, 0, 0); }
};

unsigned int hittable::index = 1; // 0 is reserved for the background

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

class rotate_y : public hittable
{
public:
    rotate_y(shared_ptr<hittable> p, double angle) : object(p)
    {
        auto radians = degree2radius(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        point3 min(infinity, infinity, infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    auto _x = cos_theta * x + sin_theta * z;
                    auto _z = -sin_theta * x + cos_theta * z;

                    vec3 _vertex(_x, y, _z);

                    for (int c = 0; c < 3; ++c)
                    {
                        min[c] = fmin(min[c], _vertex[c]);
                        max[c] = fmax(max[c], _vertex[c]);
                    }
                }

        bbox = aabb(min, max);
    }

    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        // Translate the ray into object space
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        ray rotated(origin, direction, r.time());

        // Any hits in object space?
        if (!object->hit(rotated, ray_t, hit))
            return false;

        // Translate hit point back to world space
        auto p = hit.hit_point;
        p[0] = cos_theta * hit.hit_point[0] + sin_theta * hit.hit_point[2];
        p[2] = -sin_theta * hit.hit_point[0] + cos_theta * hit.hit_point[2];

        // And also translate normal back to world space
        auto n = hit.normal;
        n[0] = cos_theta * hit.normal[0] + sin_theta * hit.normal[2];
        n[2] = -sin_theta * hit.normal[0] + cos_theta * hit.normal[2];

        hit.hit_point = p;
        hit.normal = n;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};