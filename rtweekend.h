#pragma once

#include <cmath>
#include <limits>
#include <memory>

#include <random>

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility
inline double degree2radius(double degrees) { return degrees / 180.0 * PI; }

// Return a real in [0, 1)
inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

// Return a real in [min, max)
inline double random_double(double min, double max) { return min + (max - min) * random_double(); }

// Headers
#include "interval.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"