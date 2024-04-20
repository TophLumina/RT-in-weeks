#pragma once

#include "color.h"
#include "BRDFComponents.h"

// TODO:: add support for subsurface scattering, transparency, and anisotropy

struct BRDFInfo
{
    color albedo;
    float roughness;
    float refractiveIndex;
    float metallic;
};

// Disney BRDF
class BRDF
{
public:
    virtual color operator()(const vec3 &wi, const vec3 &wo, const vec3 &n) const = 0;
};

const color defualtBaseColor = color(0.8, 0.8, 0.8);
const color defualtSpecularColor = color(1, 1, 1);

class DisneyBRDF : public BRDF
{
public:
    DisneyBRDF(const color &baseColor = defualtBaseColor, float metallic = 0, float roughness = 1)
        : baseColor(baseColor), specularColor(1), metallic(Math::clamp(metallic, 0.0f, 1.0f)), roughness(Math::clamp(roughness, 0.0f, 1.0f)) {}

    DisneyBRDF(const color &baseColor = defualtBaseColor, const color &specularColor = defualtSpecularColor, float metallic = 0, float roughness = 1)
        : baseColor(baseColor), specularColor(specularColor), metallic(metallic), roughness(roughness) {}
    
    color operator()(const vec3 &wi, const vec3 &wo, const vec3 &n) const
    {
        vec3 F0 = vec3(0.04);
        F0 = Math::Vector::linear_lerp(F0, baseColor, vec3(static_cast<double>(metallic)));

        vec3 h = normalize(wi + wo);
        float D = distributionGGX(n, h, roughness);
        float G = geometrySmith(n, wi, wo, roughness);
        vec3 F = fresnelSchlick(Math::max(dot(wi, h), 0.0), F0);

        vec3 numerator = D * G * F;
        float denominator = 4 * Math::max(dot(n, wi), 0.0) * Math::max(dot(n, wo), 0.0) + EPSILON;

        color specular = numerator / denominator;

        vec3 Ks = F;
        vec3 Kd = vec3(1.0) - Ks;
        Kd *= (1.0 - metallic);

        float NdotL = Math::max(dot(n, wi), 0.0);

        color result = (Kd * baseColor / Math::M_PI + specular) * NdotL;
        return result;
    }

private:
    color baseColor;
    color specularColor;
    float metallic;
    float roughness;
};