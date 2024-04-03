#pragma once

#include "../structures.h"

#include <initializer_list>
#include <stdexcept>

MATH_NAMESPACE_BEGIN
VECTOR_NAMESPACE_BEGIN

template <typename T>
struct vec<2, T>
{
    union
    {
        T c[2];
        T data[2];
        struct
        {
            T x, y;
        };
        struct
        {
            T r, g;
        };
        struct
        {
            T s, t;
        };
    };

    // --accessors-- //
    MATH_INLINE LENGTH_TYPE size() const
    {
        return 2;
    }

    MATH_INLINE T &at(LENGTH_TYPE i)
    {
        if (i < 0 || i >= 2)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("vec2"));
        }
        return c[i];
    }

    MATH_INLINE T const &at(LENGTH_TYPE i) const
    {
        if (i < 0 || i >= 2)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("vec2"));
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
    MATH_INLINE vec() : x(0), y(0) {}
    MATH_INLINE vec(const vec &v) = default;
    MATH_INLINE vec &operator=(const vec &v) = default;
    MATH_INLINE vec(vec &&v) = default;
    MATH_INLINE vec &operator=(vec &&v) = default;

    // --explicit conversion constructors-- //
    template <typename U>
    MATH_INLINE MATH_EXPLICIT vec(U scalar) : x(static_cast<T>(scalar)), y(static_cast<T>(scalar)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT vec(const vec<2, U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT vec(const vec<3, U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT vec(const vec<4, U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

    template <typename A, typename B>
    MATH_INLINE MATH_EXPLICIT vec(A x, B y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}

    template <typename A, typename B>
    MATH_INLINE MATH_EXPLICIT vec(const vec<1, A> &v, B y) : x(static_cast<T>(v.x)), y(static_cast<T>(y)) {}

    template <typename A, typename B>
    MATH_INLINE MATH_EXPLICIT vec(A x, const vec<1, B> &v) : x(static_cast<T>(x)), y(static_cast<T>(v.x)) {}

    // --initialization list constructors-- //
    MATH_INLINE vec(std::initializer_list<T> list)
    {
        if (list.size() != 2)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("vec2", list.size()));
        }
        auto it = list.begin();
        x = *it++;
        y = *it;
    }

    // --unary arithmetic operators-- //
    MATH_INLINE vec operator+() const
    {
        return *this;
    }

    MATH_INLINE vec operator-() const
    {
        return vec(-x, -y);
    }

    template <typename U>
    MATH_INLINE vec &operator+=(U scalar)
    {
        x += static_cast<T>(scalar);
        y += static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator+=(const vec<1, U> &v)
    {
        x += static_cast<T>(v.x);
        y += static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator+=(const vec<2, U> &v)
    {
        x += static_cast<T>(v.x);
        y += static_cast<T>(v.y);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator-=(U scalar)
    {
        x -= static_cast<T>(scalar);
        y -= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator-=(const vec<1, U> &v)
    {
        x -= static_cast<T>(v.x);
        y -= static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator-=(const vec<2, U> &v)
    {
        x -= static_cast<T>(v.x);
        y -= static_cast<T>(v.y);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator*=(U scalar)
    {
        x *= static_cast<T>(scalar);
        y *= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator*=(const vec<1, U> &v)
    {
        x *= static_cast<T>(v.x);
        y *= static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator*=(const vec<2, U> &v)
    {
        x *= static_cast<T>(v.x);
        y *= static_cast<T>(v.y);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator/=(U scalar)
    {
        x /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator/=(const vec<1, U> &v)
    {
        x /= static_cast<T>(v.x);
        y /= static_cast<T>(v.x);
        return *this;
    }

    template <typename U>
    MATH_INLINE vec &operator/=(const vec<2, U> &v)
    {
        x /= static_cast<T>(v.x);
        y /= static_cast<T>(v.y);
        return *this;
    }

    // --increment and decrement operators-- //
    MATH_INLINE vec &operator++()
    {
        static_assert(std::is_integral<T>::value, "vec2<T>::operator++(): T must be an integral type.");

        ++x;
        ++y;
        return *this;
    }

    MATH_INLINE vec operator++(int)
    {
        static_assert(std::is_integral<T>::value, "vec2<T>::operator++(int): T must be an integral type.");

        vec temp(*this);
        ++*this;
        return temp;
    }

    MATH_INLINE vec &operator--()
    {
        static_assert(std::is_integral<T>::value, "vec2<T>::operator--(): T must be an integral type.");

        --x;
        --y;
        return *this;
    }

    MATH_INLINE vec operator--(int)
    {
        static_assert(std::is_integral<T>::value, "vec2<T>::operator--(int): T must be an integral type.");

        vec temp(*this);
        --*this;
        return temp;
    }

    // TODO:: implement bit operators

    // --binary arithmetic operators-- //
    friend MATH_INLINE vec operator+(const vec &v, T scalar)
    {
        return {v.x + scalar, v.y + scalar};
    }

    friend MATH_INLINE vec operator+(T scalar, const vec &v)
    {
        return {scalar + v.x, scalar + v.y};
    }

    friend MATH_INLINE vec operator+(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x + v2.x, v1.x + v2.y};
    }

    friend MATH_INLINE vec operator+(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x + v2.x, v1.y + v2.x};
    }

    friend MATH_INLINE vec operator+(const vec &v1, const vec &v2)
    {
        return {v1.x + v2.x, v1.y + v2.y};
    }

    friend MATH_INLINE vec operator-(const vec &v, T scalar)
    {
        return {v.x - scalar, v.y - scalar};
    }

    friend MATH_INLINE vec operator-(T scalar, const vec &v)
    {
        return {scalar - v.x, scalar - v.y};
    }

    friend MATH_INLINE vec operator-(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x - v2.x, v1.x - v2.y};
    }

    friend MATH_INLINE vec operator-(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x - v2.x, v1.y - v2.x};
    }

    friend MATH_INLINE vec operator-(const vec &v1, const vec &v2)
    {
        return {v1.x - v2.x, v1.y - v2.y};
    }

    friend MATH_INLINE vec operator*(const vec &v, T scalar)
    {
        return {v.x * scalar, v.y * scalar};
    }

    friend MATH_INLINE vec operator*(T scalar, const vec &v)
    {
        return {scalar * v.x, scalar * v.y};
    }

    friend MATH_INLINE vec operator*(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x * v2.x, v1.x * v2.y};
    }

    friend MATH_INLINE vec operator*(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x * v2.x, v1.y * v2.x};
    }

    friend MATH_INLINE vec operator*(const vec &v1, const vec &v2)
    {
        return {v1.x * v2.x, v1.y * v2.y};
    }

    friend MATH_INLINE vec operator/(const vec &v, T scalar)
    {
        return {v.x / scalar, v.y / scalar};
    }

    friend MATH_INLINE vec operator/(T scalar, const vec &v)
    {
        return {scalar / v.x, scalar / v.y};
    }

    friend MATH_INLINE vec operator/(const vec<1, T> &v1, const vec &v2)
    {
        return {v1.x / v2.x, v1.x / v2.y};
    }

    friend MATH_INLINE vec operator/(const vec &v1, const vec<1, T> &v2)
    {
        return {v1.x / v2.x, v1.y / v2.x};
    }

    friend MATH_INLINE vec operator/(const vec &v1, const vec &v2)
    {
        return {v1.x / v2.x, v1.y / v2.y};
    }

    // --comparison operators-- //
    MATH_INLINE bool operator==(const vec &v) const
    {
        return x == v.x && y == v.y;
    }

    MATH_INLINE bool operator!=(const vec &v) const
    {
        return x != v.x || y != v.y;
    }
};

// --stream operators-- //
#ifdef MATH_IOS
template <typename T>
std::ostream &operator<<(std::ostream &os, const vec<2, T> &v)
{
    os << "vec2<" << typeid(T).name() << ">(" << v.x << ", " << v.y << ")";
    return os;
}

template <typename T>
std::string to_string(const vec<2, T> &v)
{
    return "vec2<" + std::string(typeid(T).name()) + ">(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
}
#endif

#ifdef MATH_TEMPLATE_ALIASES
using vec2i = vec<2, int>;
using vec2u = vec<2, unsigned>;
using vec2f = vec<2, float>;
using vec2d = vec<2, double>;
#endif

VECTOR_NAMESPACE_END
MATH_NAMESPACE_END