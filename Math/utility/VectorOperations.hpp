#pragma once

#include "../structures.h"
#include "../numeric/Numeric.hpp"


MATH_NAMESPACE_BEGIN
VECTOR_NAMESPACE_BEGIN

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> abs(vec<N, T> const &v)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = abs(v[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> random_range(vec<N, T> const &min, vec<N, T> const &max)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = random_range(min[i], max[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> min(vec<N, T> const &v, vec<N, T> const &u)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = min(v[i], u[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> max(vec<N, T> const &v, vec<N, T> const &u)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = max(v[i], u[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> floor(vec<N, T> const &v)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = floor(v[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> ceil(vec<N, T> const &v)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = ceil(v[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> round(vec<N, T> const &v)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = round(v[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS bool is_zero(vec<N, T> const &v, T const &epsilon = std::numeric_limits<T>::epsilon())
{
    if MATH_CONSTEXPR (std::is_floating_point<T>::value)
    {
        for (LENGTH_TYPE i = 0; i < N; ++i)
        {
            if (Math::abs(v[i]) > epsilon)
            {
                return false;
            }
        }
    }
    else
    {
        for (LENGTH_TYPE i = 0; i < N; ++i)
        {
            if (v[i] != 0)
            {
                return false;
            }
        }
    }
    return true;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> clamp(vec<N, T> const &v, vec<N, T> const &min, vec<N, T> const &max)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = clamp(v[i], min[i], max[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> saturate(vec<N, T> const &v)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = saturate(v[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> linear_lerp(vec<N, T> const &v, vec<N, T> const &u, vec<N, T> const &a)
{

    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = linear_lerp(v[i], u[i], a[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> quadratic_lerp(vec<N, T> const &v, vec<N, T> const &u, vec<N, T> const &a)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = quadratic_lerp(v[i], u[i], a[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> cubic_lerp(vec<N, T> const &v, vec<N, T> const &u, vec<N, T> const &a)
{
    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = cubic_lerp(v[i], u[i], a[i]);
    }
    return result;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> spherical_lerp(vec<N, T> const &v, vec<N, T> const &u, T const &a)
{
    static_assert(N == 3, "spherical lerp is only defined for 3D vectors");

    T const dot_product = dot(v, u);
    T const angle = acos(dot_product);
    T const sin_angle = sin(angle);
    return (v * sin((1 - a) * angle) + u * sin(a * angle)) / sin_angle;
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS R squared_length(vec<N, T> const &v)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    R result = 0;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result += v[i] * v[i];
    }
    return result;
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS R length(vec<N, T> const &v)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    return sqrt(squared_length<N, T, R>(v));
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS R squared_distance(vec<N, T> const &v, vec<N, T> const &u)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    return squared_length<N, T, R>(v - u);
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS R distance(vec<N, T> const &v, vec<N, T> const &u)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    return length<N, T, R>(v - u);
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS vec<N, R> normalize(vec<N, T> const &v)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    R const len = length<N, T, R>(v);
    return v / len;
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS R dot(vec<N, T> const &v, vec<N, R> const &u)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    R result = 0;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result += v[i] * u[i];
    }
    return result;
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS vec<N, R> cross(vec<N, T> const &v, vec<N, R> const &u)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");
    static_assert(N == 3, "cross product is only defined for 3D vectors");

    vec<N, R> result;
    result[0] = v[1] * u[2] - v[2] * u[1];
    result[1] = v[2] * u[0] - v[0] * u[2];
    result[2] = v[0] * u[1] - v[1] * u[0];
    return result;
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS vec<N, R> reflect(vec<N, T> const &v, vec<N, R> const &n)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    return v - 2 * dot(v, n) * n;
}

template <LENGTH_TYPE N, typename T, typename R = T>
static MATH_FUNCTION_QUALIFIERS vec<N, R> refract(vec<N, T> const &v, vec<N, R> const &n, R const &eta)
{
    static_assert(std::is_floating_point<R>::value, "R must be a floating point type");

    R const dot_product = dot(v, n);
    R const k = 1 - eta * eta * (1 - dot_product * dot_product);
    if (k < 0)
    {
        return vec<N, R>();
    }
    else
    {
        return eta * v - (eta * dot_product + sqrt(k)) * n;
    }
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> project(vec<N, T> const &v, vec<N, T> const &n)
{
    return dot(v, n) / squared_length<N, T>(n) * n;
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> reject(vec<N, T> const &v, vec<N, T> const &n)
{
    return v - project(v, n);
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> orthogonal(vec<N, T> const &v)
{
    return v - project(v, v);
}

template <LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS vec<N, T> linear2gamma(vec<N, T> const &v)
{
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

    vec<N, T> result;
    for (LENGTH_TYPE i = 0; i < N; ++i)
    {
        result[i] = Math::linear2gamma(v[i]);
    }
    return result;
}

VECTOR_NAMESPACE_END
MATH_NAMESPACE_END