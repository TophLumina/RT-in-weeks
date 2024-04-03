#pragma once

#include "../structures.h"

#include <initializer_list>

MATH_NAMESPACE_BEGIN
VECTOR_NAMESPACE_BEGIN

template <typename T>
struct vec<3, T>
{
    union
    {
        T c[3];
        T data[3];
        struct
        {
            T x, y, z;
        };
        struct
        {
            T r, g, b;
        };
        struct
        {
            T s, t, p;
        };
    };

    // --accessors-- //
    MATH_INLINE LENGTH_TYPE size() const
    {
        return 3;
    }

    MATH_INLINE T &at(LENGTH_TYPE i)
    {
        if (i < 0 || i >= 3)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("vec3"));
        }
        return c[i];
    }

    MATH_INLINE T const &at(LENGTH_TYPE i) const
    {
        if (i < 0 || i >= 3)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("vec3"));
        }
        return c[i];
    }

    // operator[] will not have out of range checks for performance
    MATH_INLINE T &operator[](LENGTH_TYPE i)
    {
        return c[i];
    }

    MATH_INLINE T const &operator[](LENGTH_TYPE i) const
    {
        return c[i];
    }

    // --implicit basic constructors-- //
    MATH_INLINE vec() : x(0), y(0), z(0) {}
    MATH_INLINE vec(const vec &v) = default;
    MATH_INLINE vec &operator=(const vec &v) = default;
    MATH_INLINE vec(vec &&v) = default;
    MATH_INLINE vec &operator=(vec &&v) = default;

    // --explicit conversion constructors-- //
    template <typename U>
    MATH_INLINE MATH_EXPLICIT vec(U scalar) : x(static_cast<T>(scalar)), y(static_cast<T>(scalar)), z(static_cast<T>(scalar)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT vec(const vec<3, U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT vec(const vec<4, U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)) {}

    template <typename A, typename B>
    MATH_INLINE MATH_EXPLICIT vec(A x, const vec<2, B> &v) : x(static_cast<T>(x)), y(static_cast<T>(v.x)), z(static_cast<T>(v.y)) {}

    template <typename A, typename B>
    MATH_INLINE MATH_EXPLICIT vec(const vec<2, A> &v, B z) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(z)) {}

    template <typename A, typename B, typename C>
    MATH_INLINE MATH_EXPLICIT vec(A x, B y, C z) : x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)) {}

    template <typename A, typename B, typename C>
    MATH_INLINE MATH_EXPLICIT vec(const vec<1, A> &v, B y, C z) : x(static_cast<T>(v.x)), y(static_cast<T>(y)), z(static_cast<T>(z)) {}

    template <typename A, typename B, typename C>
    MATH_INLINE MATH_EXPLICIT vec(A x, const vec<1, B> &v, C z) : x(static_cast<T>(x)), y(static_cast<T>(v.x)), z(static_cast<T>(z)) {}

    template <typename A, typename B, typename C>
    MATH_INLINE MATH_EXPLICIT vec(A x, B y, const vec<1, C> &v) : x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(v.x)) {}

    // --initialization list constructor-- //
    MATH_INLINE vec(std::initializer_list<T> list)
    {
        if (list.size() != 3)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("vec3", list.size()));
        }
        auto it = list.begin();
        x = *it;
        y = *(++it);
        z = *(++it);
    }

    // --unary arithmetic operators-- //
    MATH_INLINE vec operator+() const
    {
        return *this;
    }

    MATH_INLINE vec operator-() const
    {
        return vec(-x, -y, -z);
    }

    template <typename U>
    MATH_INLINE vec &operator+=(U scalar)
    {
        x += static_cast<T>(scalar);
        y += static_cast<T>(scalar);
        z += static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator+=(const vec<1, U> &v)
    {
        x += static_cast<T>(v.x);
        y += static_cast<T>(v.x);
        z += static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator+=(const vec<3, U> &v)
    {
        x += static_cast<T>(v.x);
        y += static_cast<T>(v.y);
        z += static_cast<T>(v.z);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator-=(U scalar)
    {
        x -= static_cast<T>(scalar);
        y -= static_cast<T>(scalar);
        z -= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator-=(const vec<1, U> &v)
    {
        x -= static_cast<T>(v.x);
        y -= static_cast<T>(v.x);
        z -= static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator-=(const vec<3, U> &v)
    {
        x -= static_cast<T>(v.x);
        y -= static_cast<T>(v.y);
        z -= static_cast<T>(v.z);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator*=(U scalar)
    {
        x *= static_cast<T>(scalar);
        y *= static_cast<T>(scalar);
        z *= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator*=(const vec<1, U> &v)
    {
        x *= static_cast<T>(v.x);
        y *= static_cast<T>(v.x);
        z *= static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator*=(const vec<3, U> &v)
    {
        x *= static_cast<T>(v.x);
        y *= static_cast<T>(v.y);
        z *= static_cast<T>(v.z);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator/=(U scalar)
    {
        x /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        z /= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator/=(const vec<1, U> &v)
    {
        x /= static_cast<T>(v.x);
        y /= static_cast<T>(v.x);
        z /= static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator/=(const vec<3, U> &v)
    {
        x /= static_cast<T>(v.x);
        y /= static_cast<T>(v.y);
        z /= static_cast<T>(v.z);
        return *this;
    }

    // --increment and decrement operators-- //
    MATH_INLINE vec &operator++()
    {
        static_assert(std::is_integral<T>::value, "vec3<T>::operator++(): T must be an integral type.");

        ++x;
        ++y;
        ++z;
        return *this;
    }

    MATH_INLINE vec operator++(int)
    {
        static_assert(std::is_integral<T>::value, "vec3<T>::operator++(int): T must be an integral type.");

        vec temp(*this);
        ++(*this);
        return temp;
    }

    MATH_INLINE vec &operator--()
    {
        static_assert(std::is_integral<T>::value, "vec3<T>::operator--(): T must be an integral type.");

        --x;
        --y;
        --z;
        return *this;
    }

    MATH_INLINE vec operator--(int)
    {
        static_assert(std::is_integral<T>::value, "vec3<T>::operator--(int): T must be an integral type.");

        vec temp(*this);
        --(*this);
        return temp;
    }

    // TODO:: implement bit operators

    // --binary arithmetic operators-- //
    friend MATH_INLINE vec operator+(const vec &v, T scalar)
    {
        return {v.x + scalar, v.y + scalar, v.z + scalar};
    }

    friend MATH_INLINE vec operator+(T scalar, const vec &v)
    {
        return {scalar + v.x, scalar + v.y, scalar + v.z};
    }

    friend MATH_INLINE vec operator+(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x + v2.x, v1.x + v2.y, v1.x + v2.z};
    }

    friend MATH_INLINE vec operator+(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x + v2.x, v1.y + v2.x, v1.z + v2.x};
    }

    friend MATH_INLINE vec operator+(const vec &v1, const vec &v2)
    {
        return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    }

    friend MATH_INLINE vec operator-(const vec &v, T scalar)
    {
        return {v.x - scalar, v.y - scalar, v.z - scalar};
    }

    friend MATH_INLINE vec operator-(T scalar, const vec &v)
    {
        return {scalar - v.x, scalar - v.y, scalar - v.z};
    }

    friend MATH_INLINE vec operator-(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x - v2.x, v1.x - v2.y, v1.x - v2.z};
    }

    friend MATH_INLINE vec operator-(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x - v2.x, v1.y - v2.x, v1.z - v2.x};
    }

    friend MATH_INLINE vec operator-(const vec &v1, const vec &v2)
    {
        return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    }

    friend MATH_INLINE vec operator*(const vec &v, T scalar)
    {
        return {v.x * scalar, v.y * scalar, v.z * scalar};
    }

    friend MATH_INLINE vec operator*(T scalar, const vec &v)
    {
        return {scalar * v.x, scalar * v.y, scalar * v.z};
    }

    friend MATH_INLINE vec operator*(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x * v2.x, v1.x * v2.y, v1.x * v2.z};
    }

    friend MATH_INLINE vec operator*(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x * v2.x, v1.y * v2.x, v1.z * v2.x};
    }

    friend MATH_INLINE vec operator*(const vec &v1, const vec &v2)
    {
        return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
    }

    friend MATH_INLINE vec operator/(const vec &v, T scalar)
    {
        return {v.x / scalar, v.y / scalar, v.z / scalar};
    }

    friend MATH_INLINE vec operator/(T scalar, const vec &v)
    {
        return {scalar / v.x, scalar / v.y, scalar / v.z};
    }

    friend MATH_INLINE vec operator/(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x / v2.x, v1.x / v2.y, v1.x / v2.z};
    }

    friend MATH_INLINE vec operator/(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x / v2.x, v1.y / v2.x, v1.z / v2.x};
    }

    friend MATH_INLINE vec operator/(const vec &v1, const vec &v2)
    {
        return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
    }

    // --comparison operators-- //
    friend MATH_INLINE bool operator==(const vec &v1, const vec &v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }

    friend MATH_INLINE bool operator!=(const vec &v1, const vec &v2)
    {
        return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
    }
};

// --stream operators-- //
#ifdef MATH_IOS
template <typename T>
std::ostream &operator<<(std::ostream &os, const vec<3, T> &v)
{
    os << "vec3<" << typeid(T).name() << ">(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

template <typename T>
std::string to_string(const vec<3, T> &v)
{
    return "vec3<" + std::string(typeid(T).name()) + ">(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
}
#endif

#ifdef MATH_TEMPLATE_ALIASES
using vec3i = vec<3, int>;
using vec3u = vec<3, unsigned>;
using vec3f = vec<3, float>;
using vec3d = vec<3, double>;
#endif

VECTOR_NAMESPACE_END
MATH_NAMESPACE_END