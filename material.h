#pragma once

#include "rtweekend.h"
#include "texture.h"
#include "ONB.h"
#include "PDF.h"

class scatter_info
{
public:
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
    bool no_pdf;
    ray ray_without_pdf;
};

class material
{
public:
    virtual ~material() = default;

    virtual color emitter(const ray &r_in, const hit_info &hit, double u, double v, const point3 &p) const
    {
        return color(0, 0, 0);
    }

    virtual bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const = 0;

    virtual double scattering_pdf(const ray &r_in, const hit_info &hit, const ray &scatted) const { return 0; }
};

// Diffuse material scattering weighted by cos(theta)
class lambertian : public material
{
public:
    lambertian(const shared_ptr<texture> _tex) : albedo(_tex) {}
    lambertian(const color &c) : lambertian(make_shared<solid_color>(c)) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo)const override
    {
        sinfo.attenuation = albedo->value(hit.u, hit.v, hit.hit_point);
        sinfo.pdf_ptr = make_shared<cosine_hemisphere_pdf>(hit.normal);
        sinfo.no_pdf = false;

        return true;
    }

    double scattering_pdf(const ray &r_in, const hit_info &hit, const ray &scatted) const override
    {
        auto cos_theta = dot(hit.normal, normalize(scatted.direction()));
        return cos_theta < 0 ? 0 : cos_theta / PI;
    }

private:
    shared_ptr<texture> albedo;
};

// Diffuse material scattering weighted by random directions
class diffuse : public material
{
public:
    diffuse(const shared_ptr<texture> _tex) : albedo(_tex) {}
    diffuse(const color &c) : diffuse(make_shared<solid_color>(c)) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const override
    {
        sinfo.attenuation = albedo->value(hit.u, hit.v, hit.hit_point);
        sinfo.pdf_ptr = make_shared<uniform_hemisphere_pdf>(hit.normal);
        sinfo.no_pdf = false;

        return true;
    }

    double scattering_pdf(const ray &r_in, const hit_info &hit, const ray &scatted) const override
    {
        return 1 / (2.0 * PI);
    }

private:
    shared_ptr<texture> albedo;
};

class metal : public material
{
public:
    metal(const shared_ptr<texture> _tex, double f) : albedo(_tex), fuzz(f < 1 ? f : 1) {}
    metal(const color &c, double f) : metal(make_shared<solid_color>(c), f) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const override
    {
        sinfo.attenuation = albedo->value(hit.u, hit.v, hit.hit_point);
        sinfo.pdf_ptr = nullptr;
        sinfo.no_pdf = true;

        vec3 reflected = reflect(normalize(r_in.direction()), hit.normal);
        sinfo.ray_without_pdf = ray(hit.hit_point, reflected + fuzz * random_unit_vector(), r_in.time());

        return dot(sinfo.ray_without_pdf.direction(), hit.normal) > 0; // Ensure that fuzz_scattered ray comes out
    }

private:
    shared_ptr<texture> albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    dielectric(double index_of_reflection) : ir(index_of_reflection) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const override
    {
        sinfo.attenuation = color(0.0, 0.0, 0.0); // Material like glass or water usually absorb nothing, so there is no effect on the incoming ray
        sinfo.pdf_ptr = nullptr;
        sinfo.no_pdf = true;

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

        sinfo.ray_without_pdf = ray(hit.hit_point, out, r_in.time());
        return true;
    }

private:
    double ir; // Index of Reflection
    // Common Material:     Water/Glass: 1.3 - 1.7
    //                      Diamond:     2.4

    static double reflectance(double cosine, double reflectance_index)
    {
        // Schlicks's approximation for reflectance
        auto r0 = (1 - reflectance_index) / (1 + reflectance_index);
        r0 *= r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class diffuse_light : public material
{
public:
    diffuse_light(shared_ptr<texture> e) : emit(e) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const override { return false; }

    color emitter(const ray &r_in, const hit_info &hit, double u, double v, const point3 &p) const override
    {
        if (!hit.front_face)
            return color(0, 0, 0);
        return emit->value(u, v, p);
    }

private:
    shared_ptr<texture> emit;
};

class isotropic : public material
{
public:
    isotropic(shared_ptr<texture> _tex) : albedo(_tex) {}
    isotropic(color _col) : isotropic(make_shared<solid_color>(_col)) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const override
    {
        sinfo.attenuation = albedo->value(hit.u, hit.v, hit.hit_point);
        sinfo.pdf_ptr = make_shared<uniform_sphere_pdf>();
        sinfo.no_pdf = false;

        return true;
    }

    // Samplers follow uniform distribution on sphere
    double scattering_pdf(const ray &r_in, const hit_info &hit, const ray &scatted) const override
    {
        return 1.0 / (4.0 * PI);
    }

private:
    shared_ptr<texture> albedo;
};