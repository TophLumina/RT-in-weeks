#pragma once

#include <cmath>
#include <limits>
#include <memory>

#include "Math/include/Math.h"

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();

// Utility
inline double degree2radius(double degrees) { return degrees / 180.0 * Math::M_PI; }

// Return a real in [0, 1)
inline double random_double()
{
    return Math::random_range<double>(0.0, 1.0);
}

// Return a real in [min, max)
inline double random_double(double min, double max) { return min + (max - min) * random_double(); }

// Return a integer in [min, max]
inline int random_int(int min, int max) { return static_cast<int>(random_double(min, max + 1)); }

// Headers
#include "color.h"
#include "ray.h"
#include "aabb.h"
#include "hittable.h"
#include "interval.h"
#include "vec.h"
