#pragma once

#include "vec3.h"
#include <iostream>

using color = vec3;

void write_color(std::ostream &out, color pixel_color)
{
    // color -> [0, 255]
    out << static_cast<int>(pixel_color.x() * 255.99) << ' ' <<
           static_cast<int>(pixel_color.y() * 255.99) << ' ' <<
           static_cast<int>(pixel_color.z() * 255.99) << std::endl;
}