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
        attenuation = color(1.0, 1.0, 1.0); // Material like glass or water usually absorb nothing, so there is no effect on the incoming ray
        double refraction_ratio = hit.front_face ? (1.0 / ir) : ir;

        vec3 in_normalized = normalize(r_in.direction());
        double cos_theta = fmin(dot(-in_normalized, hit.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool is_full_reflection = (refraction_ratio * sin_theta > 1.0);
        vec3 out;

        if (is_full_reflection || reflectance(cos_theta, refraction_ratio) > random_double()) // Full reflection here
        {
            out = reflect(in_normalized, hit.normal);
        }
        else // Reflection + Refraction
        {
            out = refract(in_normalized, hit.normal, refraction_ratio);
        }

        scattered = ray(hit.hit_point, out);
        return true;
    }

private:
    double ir; // Index of Reflection
    // Common Material: Water/Glass: 1.3 - 1.7
    //                  Diamond:     2.4

    static double reflectance(double cosine, double reflectance_index)
    {
        // Schlicks's approximation for reflectance
        auto r0 = (1 - reflectance_index) / (1 + reflectance_index);
        r0 *= r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};