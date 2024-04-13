#pragma once

#include "ONB.h"
#include "rtweekend.h"
#include <memory>
#include <numeric>
#include <vector>

// Any pdf class(or subclass) should be able of
// 1. Returning a random direction weighted by the internal PDF distribution
// 2. Returning the corresponding PDF distribution value in that direction
class pdf
{
public:
    virtual ~pdf(){};

    virtual double value(const vec3 &direction) const = 0;
    virtual vec3 generate() const = 0;
};

class uniform_sphere_pdf : public pdf
{
public:
    uniform_sphere_pdf() {}

    double value(const vec3 &direction) const override
    {
        return 1.0 / (4.0 * Math::M_PI);
    }

    vec3 generate() const override
    {
        return random_spherical_surface();
    }
};

class uniform_hemisphere_pdf : public pdf
{
public:
    uniform_hemisphere_pdf(const vec3 &n) { normal = n; }

    double value(const vec3 &direction) const override
    {
        return 1.0 / (2.0 * Math::M_PI);
    }

    vec3 generate() const override
    {
        return random_hemisphere_surface(normal);
    }

private:
    vec3 normal;
};

class cosine_hemisphere_pdf : public pdf
{
public:
    cosine_hemisphere_pdf(const vec3 &w) { coord.build_from_w(w); }

    double value(const vec3 &direction) const override
    {
        auto cos_theta = dot(normalize(direction), coord.w);
        return fmax(0.0, cos_theta / Math::M_PI);
    }

    vec3 generate() const override
    {
        return coord.local(random_cosine_direction());
    }

private:
    onb coord;
};

class hittable_pdf : public pdf
{
public:
    hittable_pdf(const hittable &_objects, const point3 &_origin) : objects(_objects), origin(_origin) {}

    double value(const vec3 &direction) const override
    {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override
    {
        return objects.random(origin);
    }

private:
    const hittable &objects;
    point3 origin;
};

// TODO:: use template to make it support variable number of pdfs
template <typename... Args>
class mixture_pdf : public pdf
{
public:
    // Require the same number of pdfs and weights if not default to 1.0 / num_pdfs
    mixture_pdf(std::vector<double> _weights = std::vector<double>(), Args... args)
    {
        src_pdfs = {args...};
        if (_weights.size() != src_pdfs.size())
            weights = std::vector<double>(src_pdfs.size(), 1.0 / src_pdfs.size());
        else
            weights = _weights;
        weight_sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    }

    double value(const vec3 &direction) const override
    {
        double result = 0.0;
        for (int i = 0; i < src_pdfs.size(); i++)
        {
            result += weights[i] * src_pdfs[i]->value(direction);
        }
        return result;
    }

    vec3 generate() const override
    {
        double random_num = random_double() * weight_sum;
        double sum = 0.0;
        for (int i = 0; i < src_pdfs.size(); i++)
        {
            sum += weights[i];
            if (sum > random_num)
            {
                return src_pdfs[i]->generate();
            }
        }
        return src_pdfs.back()->generate();
    }

private:
    std::vector<std::shared_ptr<pdf>> src_pdfs;
    std::vector<double> weights;
    double weight_sum;
};