#pragma once

#include "rtweekend.h"
#include "PDF.h"
#include "texture.h"
#include "BRDF.h"

#include <memory>


class scatter_info
{
public:
    BRDFInfo brdf_info;
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

    virtual bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const { return false; };

    virtual double scattering_pdf(const ray &r_in, const hit_info &hit, const ray &scatted) const { return 0; }
};

unsigned int material::index = 1; // 0 is reserved for the background

class BRDFMaterial : public material
{
public:
    BRDFMaterial(shared_ptr<texture> albedo, float roughness, float refractiveIndex, float metallic)
        : albedo(albedo), roughness(roughness), refractiveIndex(refractiveIndex), metallic(metallic) {}

    BRDFMaterial(const color& color, float roughness, float refractiveIndex, float metallic)
        : albedo(make_shared<solid_color>(color)), roughness(roughness), refractiveIndex(refractiveIndex), metallic(metallic) {}

    bool scatter(const ray &r_in, const hit_info &hit, scatter_info &sinfo) const override
    {

        sinfo.brdf_info.albedo = albedo->value(hit.u, hit.v, hit.hit_point);
        sinfo.brdf_info.roughness = roughness;
        sinfo.brdf_info.refractiveIndex = refractiveIndex;
        sinfo.brdf_info.metallic = metallic;

        sinfo.no_pdf = false;

        return true;
    }

    double scattering_pdf(const ray &r_in, const hit_info &hit, const ray &scatted) const override
    {
        if (metallic > 0.5)
        {
            return 0.0;
        }
        else
        {
            auto cos_theta = dot(hit.normal, normalize(scatted.direction()));
            return cos_theta < 0 ? 0 : cos_theta / Math::M_PI;
        }
    }
private:
    shared_ptr<texture> albedo;
    float roughness;
    float refractiveIndex;
    float metallic;
};