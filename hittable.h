#pragma once

#include "rtweekend.h"
#include "vec.h"
#include <memory>

class material;

class hit_info
{
public:
    point3 hit_point = point3(0, 0, 0);
    vec3 normal = vec3(1, 0, 0);
    shared_ptr<material> mat = nullptr;
    unsigned int hittable_index = 0; // For debugging and denoising
    double t = 0;
    double u = 0;
    double v = 0;
    bool front_face = true;

    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        // Assume that all normals have been normalized

        front_face = dot(normalize(r.direction()), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

template <typename F = double, typename V = vec3, typename M = mat4>
class transformable
{
public:
    virtual void translate(const V &offset) = 0;
    virtual void scale(const V &scalar, const V &center) = 0;
    virtual void scale(const V &scalar) = 0;
    virtual void rotate(const V &axis, F angle, const V &center) = 0;
    virtual void rotate(const V &axis, F angle) = 0;

    // Model matrix
    shared_ptr<M> m_transform = std::make_shared<M>(1.0);
};

class hittable : public transformable<>
{
public:
    unsigned int index; // For debugging and denoising
    static unsigned int total_hittables;

    hittable()
    {
        ++total_hittables;
        index = total_hittables;
    }
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_info &hit) const = 0;
    virtual aabb bounding_box() const = 0;

    virtual void translate(const vec3 &offset) override {}
    virtual void scale(const vec3 &scalar, const vec3 &center) override {}
    virtual void scale(const vec3 &scalar) override {}
    virtual void rotate(const vec3 &axis, double angle, const vec3 &center) override {}
    virtual void rotate(const vec3 &axis, double angle) override {}

    virtual double pdf_value(const point3 &origin, const vec3 &v) const { return 0.0; }
    virtual vec3 random(const vec3 &origin) const { return vec3(1, 0, 0); }
    virtual shared_ptr<material> get_material() const { return nullptr; }
    virtual vec3 get_normal(const point3 &p) const { return vec3(1, 0, 0); }

    // The Average of all the hittable's geometric centers
    // such as all vertices of a triangle, the center of a sphere, etc.
    point3 geometric_center = point3(0, 0, 0);
};

unsigned int hittable::total_hittables = 0; // 0 is reserved for the background