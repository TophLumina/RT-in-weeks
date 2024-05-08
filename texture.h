#pragma once

#include "rtweekend.h"

struct Texture
{
public:
    virtual ~Texture() = default;
    virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public Texture
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

class checker_texture : public Texture
{
public:
    checker_texture(double _scale, shared_ptr<Texture> _even, shared_ptr<Texture> _odd) : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}
    checker_texture(double _scale, color c1, color c2) : inv_scale(1.0 / _scale), even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    virtual color value(double u, double v, const point3 &p) const override
    {
        auto xi = static_cast<int>(std::floor(inv_scale * p.x));
        auto yi = static_cast<int>(std::floor(inv_scale * p.y));
        auto zi = static_cast<int>(std::floor(inv_scale * p.z));

        bool isEven = (xi + yi + zi) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};

#include "stb_impl.h"

class image_texture : public Texture
{
public:
    image_texture(const char *filename) : image(filename) {}

    color value(double u, double v, const point3 &p) const override
    {
        // If no texture data, output a debug color
        if (image.height() <= 0)
            return color(1, 0, 1);

        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v); // Flip V in tex coord

        auto x = static_cast<int>(u * image.width());
        auto y = static_cast<int>(v * image.height());
        auto pixel = image.pixel_data(x, y);

        auto color_scale = 1.0 / 255.0;
        return color_scale * color(pixel[0], pixel[1], pixel[2]);
    }

private:
    rtw_image image;
};

#include "perlin.h"

class noise_texture : public Texture
{
public:
    noise_texture(double _scale) : scale(_scale) {}

    color value(double u, double v, const point3 &p) const override
    {
        auto s = scale * p;
        // turbulence in other math functions to make repeatable noise pattern
        return color(1, 1, 1) * 0.5 * (1 + sin(s.z + 10 * noise.turb(s)));
    }

private:
    perlin noise;

    // Frequency
    double scale;
};