#pragma once

#include "vec3.h"
#include <iostream>

using color = vec3;

void write_color(std::ostream &out, color pixel_color, int samplers_per_pixel)
{
    color col = pixel_color;

    col /= samplers_per_pixel;

    auto r = col.x();
    auto g = col.y();
    auto b = col.z();

    static const interval intensity(0.000, 0.999);

    // color -> [0, 255]
    out << static_cast<int>(255.99 * intensity.clamp(r)) << ' '
        << static_cast<int>(255.99 * intensity.clamp(g)) << ' '
        << static_cast<int>(255.99 * intensity.clamp(b)) << '\n';
}