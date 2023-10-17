#pragma once

#include "rtweekend.h"

class texture
{
public:
    virtual ~texture() = default;
    virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture
{
public:
    solid_color(color c) : color_value(c) {}
    solid_color(double r, double g, double b) : solid_color(color(r, g, b)) {}
    color value(double u, double v, const point3 &p) const
    {
        return color_value;
    }

private:
    color color_value;
};

class checker_texture : public texture
{
public:
    checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd) : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}
    checker_texture(double _scale, color c1, color c2) : checker_texture(_scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

    virtual color value(double u, double v, const point3 &p) const override
    {
        auto xi = static_cast<int>(std::floor(inv_scale * p.x()));
        auto yi = static_cast<int>(std::floor(inv_scale * p.y()));
        auto zi = static_cast<int>(std::floor(inv_scale * p.z()));

        bool isEven = (xi + yi + zi) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};