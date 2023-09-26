#pragma once

#include "rtweekend.h"

class hit_info;

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray &r_in, const hit_info &hit, color &attenuation, ray &scattered) const = 0;
};

class lambertian : public material
{
public:
    lambertian(const color &a) : albedo(a) {}

    bool scatter(const ray &r_in, const hit_info &hit, color &attenuation, ray &scattered) const override
    {
        vec3 scatter_direction = hit.normal + random_unit_vector();

        // When random_vector is in the opposite of normal, it will make scatter_direction zero
        if (scatter_direction.near_zero())
            scatter_direction = hit.normal;

        scattered = ray(hit.hit_point, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

class metal : public material
{
public:
    metal(const color &a) : albedo(a) {}

    bool scatter(const ray &r_in, const hit_info &hit, color &attenuation, ray &scattered) const override
    {
        vec3 reflected = reflect(normalize(r_in.direction()), hit.normal);
        scattered = ray(hit.hit_point, reflected);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};