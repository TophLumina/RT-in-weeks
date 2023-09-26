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
    metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray &r_in, const hit_info &hit, color &attenuation, ray &scattered) const override
    {
        vec3 reflected = reflect(normalize(r_in.direction()), hit.normal);
        scattered = ray(hit.hit_point, reflected + fuzz * random_unit_vector());
        attenuation = albedo;
        return dot(scattered.direction(), hit.normal) > 0; // Ensure that fuzz_scattered ray comes out
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    dielectric(double index_of_reflection) : ir(index_of_reflection) {}

    bool scatter(const ray &r_in, const hit_info &hit, color &attenuation, ray &scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = hit.front_face ? (1.0 / ir) : ir;

        // TODO::Finish Glass Material
    }

private:
    double ir; // Index of Reflection
};