#pragma once

#include "ONB.h"
#include <memory>
#include <numeric>
#include <vector>

// Any pdf class(or subclass) should be able of
// 1. Returning a random direction weighted by the internal PDF distribution
// 2. Returning the corresponding PDF distribution value in that direction

template<typename F = double>
class pdf
{
public:
    virtual ~pdf(){};

    virtual F value(const vec3 &direction) const = 0;
    virtual vec3 generate() const = 0;
};

template<typename F = double>
class uniform_sphere_pdf : public pdf<F>
{
public:
    uniform_sphere_pdf() {}

    F value(const vec3 &direction) const override
    {
        return 1.0 / (4.0 * Math::M_PI);
    }

    vec3 generate() const override
    {
        return random_spherical_surface();
    }
};

template<typename F = double>
class uniform_hemisphere_pdf : public pdf<F>
{
public:
    uniform_hemisphere_pdf(const vec3 &n) { normal = n; }

    F value(const vec3 &direction) const override
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

template<typename F = double>
class cosine_hemisphere_pdf : public pdf<F>
{
public:
    cosine_hemisphere_pdf(const vec3 &w) { coord.build_from_w(w); }

    F value(const vec3 &direction) const override
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

template<typename F = double>
class hittable_pdf : public pdf<F>
{
public:
    hittable_pdf(const hittable &_objects, const point3 &_origin) : objects(_objects), origin(_origin) {}

    F value(const vec3 &direction) const override
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

template <typename F = double, typename... PDFs>
class mixture_pdf : public pdf<F>
{
public:
    // Require the same number of pdfs and weights if not default to 1.0 / num_pdfs
    mixture_pdf(std::vector<F> _weights = std::vector<F>(), PDFs... pdfs)
    {
        src_pdfs = {pdfs...};

        // check if the number of weights is the same as the number of pdfs
        if (_weights.size() != src_pdfs.size())
        {
            weights = std::vector<F>(src_pdfs.size(), 1.0 / src_pdfs.size());
        }
        else
        {
            weights = _weights;
        }
        weight_sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    }

    F value(const vec3 &direction) const override
    {
        F result = 0.0;
        for (int i = 0; i < src_pdfs.size(); i++)
        {
            result += weights[i] * src_pdfs[i]->value(direction);
        }
        return result / weight_sum;
    }

    vec3 generate() const override
    {
        F random_num = random_double() * weight_sum;
        F sum = 0.0;
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
    std::vector<std::shared_ptr<pdf<F>>> src_pdfs;
    std::vector<F> weights;
    F weight_sum;
};