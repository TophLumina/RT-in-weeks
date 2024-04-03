#pragma once

#include "../Config.h"
#include <cmath>

MATH_NAMESPACE_BEGIN

const double M_PI = acos(-1);
const double M_PI_2 = M_PI / 2;
const double M_PI_4 = M_PI / 4;
const double M_1_PI = 1 / M_PI;
const double M_2_PI = 2 / M_PI;
const double M_2_SQRTPI = 2 / sqrt(M_PI);
const double M_SQRT2PI = sqrt(2 * M_PI);
const double M_SQRT1_2PI = sqrt(M_PI / 2);

const double M_SQRT2 = sqrt(2);
const double M_SQRT1_2 = sqrt(0.5);
const double M_SQRT3 = sqrt(3);

const double M_E = exp(1);
const double M_LOG2E = log2(M_E);
const double M_LOG10E = log10(M_E);
const double M_LN2 = log(M_E);
const double M_LN10 = log10(M_E);

MATH_NAMESPACE_END