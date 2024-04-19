#pragma once

#include "vec3.h"
#include "color.h"

// TODO:: add support for subsurface scattering, transparency, and anisotropy

// n: normal, h: half vector (both normalized)

float distributionBeckmann(const vec3& n, const vec3& h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = Math::max(dot(n, h), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = exp((NdotH2 - 1.0f) / (a2 * NdotH2));
    float denom = Math::M_PI * a2 * NdotH2 * NdotH2;

    return nom / denom;
}

float distributionGGX(const vec3 &n, const vec3 &h, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = Math::max(dot(n, h), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = Math::M_PI * denom * denom;

    return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float geometrySmith(const vec3 &n, const vec3 &v, const vec3 &l, float roughness)
{
    float NdotV = Math::max(dot(n, v), 0.0);
    float NdotL = Math::max(dot(n, l), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float fresnelSchlick(float cosTheta, float refractiveIndex)
{
    float F0 = pow((1 - refractiveIndex) / (1 + refractiveIndex), 2);
    return F0 + (1 - F0) * pow(1 - cosTheta, 5);
}


// Disney BRDF
class BRDF
{
public:
    virtual color operator()(const vec3 &wi, const vec3 &wo, const vec3 &n) const = 0;
};

class DisneyBRDF : public BRDF
{
public:
    DisneyBRDF(const color &baseColor, float metallic, float roughness)
        : baseColor(baseColor), metallic(metallic), roughness(roughness) {}

    color operator()(const vec3 &wi, const vec3 &wo, const vec3 &n) const
    {
        vec3 h = normalize(wi + wo);
        float D = distributionGGX(n, h, roughness);
        float G = geometrySmith(n, wi, wo, roughness);
        float F = fresnelSchlick(Math::max(dot(wi, h), 0.0));

        color specular = (D * G * F) / (4 * Math::max(dot(n, wi), 0.0) * Math::max(dot(n, wo), 0.0));
        color diffuse = baseColor / Math::M_PI;

        return Math::Vector::linear_lerp(diffuse, specular, vec3(metallic));
    }

private:
    color baseColor;
    float metallic;
    float roughness;

    float fresnelSchlick(float cosTheta) const
    {
        return metallic + (1 - metallic) * pow(1 - cosTheta, 5);
    }
};


struct BRDFInfo
{
    color albedo;
    float roughness;
    float refractiveIndex;
    float metallic;
};