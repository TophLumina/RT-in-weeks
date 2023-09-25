#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility
inline double degree2radius(double degrees) { return degrees / 180.0 * PI; }

inline double random_double() { return rand() / (RAND_MAX + 1.0); } // Return a real in [0, 1)

inline double random_double(double min, double max) { return min + (max - min) * random_double(); } // Return a real in [min, max)

// Headers
#include "interval.h"
#include "vec3.h"
#include "ray.h"