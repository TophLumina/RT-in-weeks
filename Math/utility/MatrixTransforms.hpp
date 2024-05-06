#pragma once

#include "../numeric/Numeric.hpp"
#include "VectorOperations.hpp"
#include "MatrixOperations.hpp"

MATH_NAMESPACE_BEGIN
MATRIX_NAMESPACE_BEGIN

// 3D rendering specified Matrix functions

template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> translate(Vector::vec<3, T> const &v)
{
    mat<4, 4, T> result = identity<4, 4, T>();
    result[3] = Vector::vec<4, T>(v, static_cast<T>(1));
    return result;
}

template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> rotate(Vector::vec<3, T> const &_axis, T angle)
{
    Vector::vec<3, T> axis = Vector::normalize(_axis);
    T const cos = Math::cos(angle);
    T const sin = Math::sin(angle);
    T const invCos = static_cast<T>(1) - cos;

    mat<4, 4, T> result = identity<4, 4, T>();
    result[0][0] = cos + axis[0] * axis[0] * invCos;
    result[1][0] = axis[0] * axis[1] * invCos - axis[2] * sin;
    result[2][0] = axis[0] * axis[2] * invCos + axis[1] * sin;
    result[0][1] = axis[1] * axis[0] * invCos + axis[2] * sin;
    result[1][1] = cos + axis[1] * axis[1] * invCos;
    result[2][1] = axis[1] * axis[2] * invCos - axis[0] * sin;
    result[0][2] = axis[2] * axis[0] * invCos - axis[1] * sin;
    result[1][2] = axis[2] * axis[1] * invCos + axis[0] * sin;
    result[2][2] = cos + axis[2] * axis[2] * invCos;
    return result;
}

template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> rotate(const Vector::vec<3, T> &_axis, T angle, const Vector::vec<3, T> &center)
{
    return translate(center) * rotate(_axis, angle) * translate(-center);
}

template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> scale(Vector::vec<3, T> const &v)
{
    mat<4, 4, T> result(1);
    result[0][0] = v[0];
    result[1][1] = v[1];
    result[2][2] = v[2];
    return result;
}

template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> scale(const Vector::vec<3, T> &v, const Vector::vec<3, T> &center)
{
    return translate(center) * scale(v) * translate(-center);
}

template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> lookAt(Vector::vec<3, T> const &eye, Vector::vec<3, T> const &center, Vector::vec<3, T> const &up)
{
    Vector::vec<3, T> f = Vector::normalize(center - eye);
    Vector::vec<3, T> u = Vector::normalize(up);
    Vector::vec<3, T> s = Vector::normalize(Vector::cross(f, u));
    u = Vector::cross(s, f);

    mat<4, 4, T> result = identity<4, 4, T>();
    result[0][0] = s[0];
    result[1][0] = s[1];
    result[2][0] = s[2];
    result[0][1] = u[0];
    result[1][1] = u[1];
    result[2][1] = u[2];
    result[0][2] = -f[0];
    result[1][2] = -f[1];
    result[2][2] = -f[2];
    result[3][0] = -Math::Vector::dot(s, eye);
    result[3][1] = -Math::Vector::dot(u, eye);
    result[3][2] = Math::Vector::dot(f, eye);
    return result;
}

// apply orthographic projection but keep z value unchanged
template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> ortho(T left, T right, T bottom, T top)
{
    mat<4, 4, T> result = identity<4, 4, T>();
    result[0][0] = static_cast<T>(2) / (right - left);
    result[1][1] = static_cast<T>(2) / (top - bottom);
    result[2][2] = static_cast<T>(1);
    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    return result;
}

// left-handed in clip space [OpenGL], z value mapped to the range [0, 1]
// if you want to use right-handed, you can manually scale the z axis by -1
template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> orthoLH_ZO(T left, T right, T bottom, T top, T zNear, T zFar)
{
    mat<4, 4, T> result = identity<4, 4, T>();
    result[0][0] = static_cast<T>(2) / (right - left);
    result[1][1] = static_cast<T>(2) / (top - bottom);
    result[2][2] = static_cast<T>(1) / (zFar - zNear);
    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -zNear / (zFar - zNear);
    return result;
}

// left-handed in clip space [OpenGL], z value mapped to the range [-1, 1]
// if you want to use right-handed, you can manually scale the z axis by -1
template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> orthoLH_NO(T left, T right, T bottom, T top, T zNear, T zFar)
{
    mat<4, 4, T> result = identity<4, 4, T>();
    result[0][0] = static_cast<T>(2) / (right - left);
    result[1][1] = static_cast<T>(2) / (top - bottom);
    result[2][2] = -static_cast<T>(2) / (zFar - zNear);
    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -(zFar + zNear) / (zFar - zNear);
    return result;
}

// left-handed in clip space [OpenGL], z value mapped to the range [0, 1]
// if you want to use right-handed, you can manually scale the z axis by -1
template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> frustumLH_ZO(T left, T right, T bottom, T top, T zNear, T zFar)
{
    mat<4, 4, T> result(0);
    result[0][0] = static_cast<T>(2) * zNear / (right - left);
    result[1][1] = static_cast<T>(2) * zNear / (top - bottom);
    result[2][0] = (right + left) / (right - left);
    result[2][1] = (top + bottom) / (top - bottom);
    result[2][2] = zFar / (zFar - zNear);
    result[2][3] = static_cast<T>(1);
    result[3][2] = -(zNear * zFar) / (zFar - zNear);
    return result;
}

// left-handed in clip space [OpenGL], z value mapped to the range [-1, 1]
// if you want to use right-handed, you can manually scale the z axis by -1
template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> frustumLH_NO(T left, T right, T bottom, T top, T zNear, T zFar)
{
    mat<4, 4, T> result(0);
    result[0][0] = static_cast<T>(2) * zNear / (right - left);
    result[1][1] = static_cast<T>(2) * zNear / (top - bottom);
    result[2][0] = (right + left) / (right - left);
    result[2][1] = (top + bottom) / (top - bottom);
    result[2][2] = (zFar + zNear) / (zFar - zNear);
    result[2][3] = static_cast<T>(1);
    result[3][2] = -(static_cast<T>(2) * zNear * zFar) / (zFar - zNear);
    return result;
}

// left-handed in clip space [OpenGL], z value mapped to the range [0, 1]
// if you want to use right-handed, you can manually scale the z axis by -1
template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> perspectiveLH_ZO(T fovy, T aspect, T zNear, T zFar)
{
    if (abs(aspect - std::numeric_limits<T>::epsilon()) < static_cast<T>(0))
    {
        throw std::runtime_error("aspect ratio cannot be zero");
    }

    T const tanHalfFovy = tan(fovy / static_cast<T>(2));

    mat<4, 4, T> result(0);
    result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
    result[1][1] = static_cast<T>(1) / (tanHalfFovy);
    result[2][2] = zFar / (zFar - zNear);
    result[2][3] = static_cast<T>(1);
    result[3][2] = -(zNear * zFar) / (zFar - zNear);
    return result;
}

// left-handed in clip space [OpenGL], z value mapped to the range [-1, 1]
// if you want to use right-handed, you can manually scale the z axis by -1
template <typename T>
static MATH_FUNCTION_QUALIFIERS mat<4, 4, T> perspectiveLH_NO(T fovy, T aspect, T zNear, T zFar)
{
    if (Math::abs(aspect - std::numeric_limits<T>::epsilon()) < static_cast<T>(0))
    {
        throw std::runtime_error("aspect ratio cannot be zero");
    }

    T const tanHalfFovy = tan(fovy / static_cast<T>(2));

    mat<4, 4, T> result(0);
    result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
    result[1][1] = static_cast<T>(1) / (tanHalfFovy);
    result[2][2] = (zFar + zNear) / (zFar - zNear);
    result[2][3] = static_cast<T>(1);
    result[3][2] = -(static_cast<T>(2) * zNear * zFar) / (zFar - zNear);
    return result;
}

MATRIX_NAMESPACE_END
MATH_NAMESPACE_END