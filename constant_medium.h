#pragma once

#include "rtweekend.h"

#include "material.h"
#include "texture.h"

class constant_medium : public hittable
{
public:
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> tex) : boundary(b), neg_inv_density(-1.0 / d), phase_function(make_shared<isotropic>(tex)) {}

    constant_medium(shared_ptr<hittable> b, double d, color c) : boundary(b), neg_inv_density(-1.0 / d), phase_function(make_shared<isotropic>(c)) {}

    bool hit(const ray &r, interval ray_t, hit_info &hit) const override
    {
        hit_info hit_in, hit_out;

        // Query hits through the boundary
        if (!boundary->hit(r, universe, hit_in))
            return false;
        if (!boundary->hit(r, interval(hit_in.t + 1e-4, infinity), hit_out))
            return false;

        // debug info waiting to be implemented

        // Clamp to ray_t
        hit_in.t = hit_in.t < ray_t.min ? ray_t.min : hit_in.t;
        hit_out.t = hit_out.t > ray_t.max ? ray_t.max : hit_out.t;

        if (hit_out.t < hit_in.t)
            return false;

        hit_in.t = hit_in.t < 0 ? 0 : hit_in.t;

        // Scatting distance
        auto ray_len = length(r.direction());
        auto dist_inside_boundary = (hit_out.t - hit_in.t) * ray_len;
        auto hit_dist = neg_inv_density * log(random_double());

        if (hit_dist > dist_inside_boundary)
            return false;

        // hittable index
        hit.hittable_index = boundary->index;
        
        // Scatting confirmed
        hit.t = hit_in.t + dist_inside_boundary / length(r.direction());
        hit.hit_point = r.at(hit.t);

        hit.normal = vec3(1, 0, 0); // arbitrary
        hit.front_face = true;      // also arbitrary
        hit.mat = phase_function;

        return true;
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