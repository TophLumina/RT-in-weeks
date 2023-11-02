#pragma once

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable
{
public:
    constant_medium()
    {

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
    }

private:
    shared_ptr<texture> albedo;
};