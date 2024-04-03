#pragma once

#include <cmath>

#include "./Math/include/Math.h"

using namespace Math::Vector;

using vec3 = vec3d;
using point3 = vec3;

// Sampling direction weighted by cosine on upper hemisphere (z+)
// PDF = cos(phi) / PI or dir.z / PI or dot(w, dir) / PI
inline vec3 random_cosine_direction()
{
    auto r1 = Math::random_range(0.0, 1.0);
    auto r2 = Math::random_range(0.0, 1.0);

    auto phi = 2 * Math::M_PI * r1;

    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);
    auto z = sqrt(1 - r2);

    return vec3(x, y, z);
}