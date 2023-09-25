#pragma once

#include <cmath>
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

// Headers
#include "interval.h"
#include "vec3.h"
#include "ray.h"