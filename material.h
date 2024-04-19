#pragma once

#include "BRDF.h"
#include "PDF.h"
#include "rtweekend.h"
#include "texture.h"
#include <memory>

class scatter_info
{
public:
    BRDFInfo brdf_info;
    std::shared_ptr<pdf> brdf_pdf;
    bool no_pdf;
    ray ray_without_pdf;
};

class material
{
public:
    static unsigned int index; // For debugging and denoising

    material() { ++index; }
    virtual ~material() = default;

    virtual color emitter(const ray &r_in, const hit_info &hit, double u, double v, const point3 &p) const
    {
        return color(0, 0, 0);
    }

    // Returns true if the ray is scattered, and stores scatter pdf in sinfo
    virtual bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const { return false; };

    virtual color scatter_color(const ray &r_in, const hit_info &hit, const ray &scattered) const { return color(0, 0, 0); }
};

unsigned int material::index = 1; // 0 is reserved for the background

class DisneyMaterial : public material
{
public:
    DisneyMaterial(shared_ptr<texture> albedo, float roughness, float refractiveIndex, float metallic)
        : albedo(albedo), roughness(roughness), refractiveIndex(refractiveIndex), metallic(metallic) {}

    DisneyMaterial(const color &color, float roughness, float refractiveIndex, float metallic)
        : albedo(make_shared<solid_color>(color)), roughness(roughness), refractiveIndex(refractiveIndex), metallic(metallic) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const override
    {

        sinfo.brdf_info.albedo = albedo->value(hit.u, hit.v, hit.hit_point);
        sinfo.brdf_info.roughness = roughness;
        sinfo.brdf_info.refractiveIndex = refractiveIndex;
        sinfo.brdf_info.metallic = metallic;

        // for Disney BRDF, we use GGX for NDC
        sinfo.brdf_pdf = make_shared<GGX_pdf>(hit.normal, roughness);
        sinfo.no_pdf = false;

        return true;
    }

    color scatter_color(const ray &r_in, const hit_info &hit, const ray &scattered) const override
    {
        color baseColor = albedo->value(hit.u, hit.v, hit.hit_point);
        float metallic = this->metallic;
        float roughness = this->roughness;

        vec3 wi = normalize(r_in.direction());
        vec3 wo = normalize(scattered.direction());
        vec3 n = hit.normal;

        DisneyBRDF brdf(baseColor, metallic, roughness);
        return brdf(wi, wo, n);
    }

private:
    shared_ptr<texture> albedo;
    float roughness;
    float refractiveIndex;
    float metallic;
};

// It emits only when the ray hits the front face
class EmissiveMaterial : public material
{
public:
    EmissiveMaterial(shared_ptr<texture> emit) : emit(emit) {}
    EmissiveMaterial(const color &color) : emit(make_shared<solid_color>(color)) {}

    color emitter(const ray &r_in, const hit_info &hit, double u, double v, const point3 &p) const override
    {
        if (hit.front_face)
            return emit->value(u, v, p);
        else
            return color(0, 0, 0);
    }

private:
    shared_ptr<texture> emit;
};