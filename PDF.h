#pragma once

#include "rtweekend.h"
#include "ONB.h"
#include "hittable_list.h"

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
        return 1.0 / (4.0 * PI);
    }

    vec3 generate() const override
    {
        return random_unit_vector();
    }
};

class uniform_hemisphere_pdf : public pdf
{
public:
    uniform_hemisphere_pdf(const vec3 &n) { normal = n; }

    double value(const vec3 &direction) const override
    {
        return 1.0 / (2.0 * PI);
    }

    vec3 generate() const override
    {
        return random_on_hemisphere(normal);
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
        auto cos_theta = dot(normalize(direction), coord.w());
        return fmax(0.0, cos_theta / PI);
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

class mixture_pdf : public pdf
{
public:
    // when getting mixed_pdf value, it will follow by _mix_param * p0 + (1.0 - _mix_param) * p1
    mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1, double _mix_param = 0.5)
    {
        src_pdf[0] = p0;
        src_pdf[1] = p1;
        mix_param = std::max(std::min(1.0, _mix_param), 0.0); // clamp to [0.0, 1.0]
    }

    double value(const vec3 &direction) const override
    {
        return mix_param * src_pdf[0]->value(direction) + (1.0 - mix_param) * src_pdf[1]->value(direction);
    }

    vec3 generate() const override
    {
        if (random_double() < mix_param)
            return src_pdf[0]->generate();
        else
            return src_pdf[1]->generate();
    }

private:
    shared_ptr<pdf> src_pdf[2];
    double mix_param;
};