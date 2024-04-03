#pragma once
#include "../structures.h"
#include "Numeric.hpp"


#include <random>

MATH_NAMESPACE_BEGIN

template <typename T>
static MATH_INLINE T random_range(T const &min = std::numeric_limits<T>::min(), T const &max = std::numeric_limits<T>::max())
{
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<T> dis(min, max);
    return dis(gen);
}

template <typename T>
static MATH_INLINE T random_gaussian(T const &mean = static_cast<T>(0), T const &stddev = static_cast<T>(1))
{
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<T> dis(mean, stddev);
    return dis(gen);
}

VECTOR_NAMESPACE_BEGIN
template <LENGTH_TYPE N = 2, typename T = float>
static MATH_INLINE vec<N, T> random_disk(T const &radius = static_cast<T>(1))
{
    static_assert(N == 2, "random disk is only defined for 2D vectors");
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

    T const angle = Math::random_range(static_cast<T>(0), static_cast<T>(2 * M_PI));
    T const r = Math::random_range(static_cast<T>(0), radius);
    return vec<N, T>(r * cos(angle), r * sin(angle));
}

template <LENGTH_TYPE N = 3, typename T = float>
static MATH_INLINE vec<N, T> random_spherical(T const &radius = static_cast<T>(1))
{
    static_assert(N == 3, "random sphere is only defined for 3D vectors");
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

    T const theta = Math::random_range(static_cast<T>(0), static_cast<T>(2 * M_PI));
    T const phi = Math::random_range(static_cast<T>(0), static_cast<T>(M_PI));
    T const r = Math::random_range(static_cast<T>(0), radius);
    return vec<N, T>(r * sin(phi) * cos(theta), r * sin(phi) * sin(theta), r * cos(phi));
}

template <LENGTH_TYPE N = 3, typename T = float>
static MATH_INLINE vec<N, T> random_spherical_surface(T const &radius = static_cast<T>(1))
{
    static_assert(N == 3, "random sphere is only defined for 3D vectors");
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

    T const theta = Math::random_range(static_cast<T>(0), static_cast<T>(2 * M_PI));
    T const phi = Math::random_range(static_cast<T>(0), static_cast<T>(M_PI));
    return vec<N, T>(radius * sin(phi) * cos(theta), radius * sin(phi) * sin(theta), radius * cos(phi));
}

template <LENGTH_TYPE N = 3, typename T = float>
static MATH_INLINE vec<N, T> random_hemisphere(vec<N, T> const &normal, T const &radius = static_cast<T>(1))
{
    static_assert(N == 3, "random hemisphere is only defined for 3D vectors");
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

    vec<N, T> const v = random_spherical(radius);
    return dot(v, normal) > 0 ? v : -v;
}

template <LENGTH_TYPE N = 3, typename T = float>
static MATH_INLINE vec<N, T> random_hemisphere_surface(vec<N, T> const &normal, T const &radius = static_cast<T>(1))
{
    static_assert(N == 3, "random hemisphere is only defined for 3D vectors");
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

    vec<N, T> const v = random_spherical_surface(radius);
    return dot(v, normal) > 0 ? v : -v;
}
VECTOR_NAMESPACE_END
MATH_NAMESPACE_END