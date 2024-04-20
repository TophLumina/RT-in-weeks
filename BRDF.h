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
        : baseColor(baseColor), specularColor(1), metallic(metallic), roughness(roughness) {}

    DisneyBRDF(const color &baseColor = defualtBaseColor, const color &specularColor = defualtSpecularColor, float metallic = 0, float roughness = 1)
        : baseColor(baseColor), specularColor(specularColor), metallic(metallic), roughness(roughness) {}
    
    color operator()(const vec3 &wi, const vec3 &wo, const vec3 &n) const
    {
        vec3 h = normalize(wi + wo);
        float D = distributionGGX(n, h, roughness);
        float G = geometrySmith(n, wi, wo, roughness);
        vec3 F = fresnelSchlick(Math::max(dot(wi, h), 0.0));

        color specular = specularColor * (D * G * F) / (4 * dot(n, wi) * dot(n, wo));
        color diffuse = baseColor / Math::M_PI;

        color result = linear_lerp(diffuse, specular, vec3(metallic));
        return result * Math::max(dot(n, wi), 0.0);
    }

private:
    color baseColor;
    color specularColor;
    float metallic;
    float roughness;

    vec3 fresnelSchlick(float cosTheta, const color &F0 = color(0.04, 0.04, 0.04)) const
    {
        return F0 + (color(1, 1, 1) - F0) * pow(1 - cosTheta, 5);
    }
};