#pragma once

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable
{
public:
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> tex) : boundary(b), neg_inv_density(-1.0 / d), phase_function(make_shared<isotropic>(tex)) {}

    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<color> c) : boundary(b), neg_inv_density(-1.0 / d), phase_function(make_shared<isotropic>(c)) {}

    bool hittable::hit(const ray &r, interval ray_t, hit_info &hit) const override
    {

        hit_info hit_in, hit_out;

        // Query hits through the boundary
        if (!boundary->hit(r, interval::universe, hit_in))
            return false;
        if (!boundary->hit(r,interval(hit_in.t+1e-4,infinity),hit_out))
            return false;
        
        // debug


        // Clamp to ray_t
        hit_in.t = hit_in.t < ray_t.min ? ray_t.min : hit_in.t;
        hit_out.t = hit_out.t > ray_t.max ? ray_t.max : hit_out.t;

        if (hit_out.t < hit_in.t)
            return false;

        hit_in.t = hit_in.t < 0 ? 0 : hit_in.t;

        // Scatting distance
        auto ray_len = r.direction().length();
        auto dist_inside_boundary = (hit_out.t - hit_in.t) * ray_len;
        auto hit_dist = neg_inv_density * log(random_double());

        if (hit_dist > dist_inside_boundary)
            return false;
        
        // Scatting confirmed
        
    }

    aabb bounding_box() const override
    {
        return boundary->bounding_box();
    }
private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;
};

class isotropic : public material
{
public:
    isotropic(shared_ptr<texture> _tex) : albedo(_tex) {}
    isotropic(color _col) : isotropic(make_shared<solid_color>(_col)) {}

    bool scatter(const ray &r_in, const hit_info &hit, color &attenuation, ray &scattered) const override
    {
        scattered = ray(hit.hit_point, random_unit_vector(), r_in.time());
        attenuation = albedo->value(hit.u, hit.v, hit.hit_point);

        return true;
    }

private:
    shared_ptr<texture> albedo;
};