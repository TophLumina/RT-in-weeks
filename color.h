#pragma once

#include "vec3.h"
#include <iostream>

using color = vec3;

inline double linear2gamma(double linear_component) { return sqrt(linear_component); } // Gamma Correction (sqrt(x) is slightly faster than pow(x, .45))

void write_color(std::ostream &out, color pixel_color, int samplers_per_pixel)
{
    color col = pixel_color;

    col /= samplers_per_pixel;

    auto r = col.x();
    auto g = col.y();
    auto b = col.z();

    r = linear2gamma(r);
    g = linear2gamma(g);
    b = linear2gamma(b);

    static const interval intensity(0.000, 0.999);

    // color -> [0, 255]
    out << static_cast<int>(255.99 * intensity.clamp(r)) << ' '
        << static_cast<int>(255.99 * intensity.clamp(g)) << ' '
        << static_cast<int>(255.99 * intensity.clamp(b)) << '\n';
}