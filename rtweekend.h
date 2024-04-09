#pragma once

#include <cmath>
#include <limits>
#include <random>

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
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_real_distribution<> distribution(0.0, 1.0);
    return distribution(generator);
}

// Return a real in [min, max)
inline double random_double(double min, double max) { return min + (max - min) * random_double(); }

// Return a integer in [min, max]
inline int random_int(int min, int max) { return static_cast<int>(random_double(min, max + 1)); }

// Headers
#include "color.h"
#include "hittable.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
