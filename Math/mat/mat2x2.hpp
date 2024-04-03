#pragma once

#include "../Config.h"
#include "../structures.h"

#include <initializer_list>

MATH_NAMESPACE_BEGIN
MATRIX_NAMESPACE_BEGIN

template <typename T>
struct mat<2, 2, T>
{
    union
    {
        Vector::vec<2, T> c[2];
        T data[4];
        struct
        {
            // Stored in column major order
            // source matrix:
            // | m00 m10 |
            // | m01 m11 |
            T m00, m01;
            T m10, m11;
        };
    };

    // --accessors-- //
    MATH_INLINE LENGTH_TYPE size() const
    {
        return 2;
    }

    MATH_INLINE Vector::vec<2, T> &at(LENGTH_TYPE i)
    {
        if (i < 0 || i >= 2)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("mat2x2"));
        }
        return c[i];
    }

    MATH_INLINE Vector::vec<2, T> const &at(LENGTH_TYPE i) const
    {
        if (i < 0 || i >= 2)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("mat2x2"));
        }
        return c[i];
    }

    // operator[] will not have out of range checks for performance
    MATH_INLINE Vector::vec<2, T> &operator[](LENGTH_TYPE i)
    {
        return c[i];
    }

    MATH_INLINE Vector::vec<2, T> const &operator[](LENGTH_TYPE i) const
    {
        return c[i];
    }

    // --implicit basic constructors-- //
    MATH_INLINE mat() : m00(0), m01(0),
                        m10(0), m11(0) {}
    MATH_INLINE mat(const mat &m) = default;
    MATH_INLINE mat &operator=(const mat &m) = default;
    MATH_INLINE mat(mat &&m) = default;
    MATH_INLINE mat &operator=(mat &&m) = default;

    // --explicit conversion constructors-- //
    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(U scalar) : m00(static_cast<T>(scalar)), m01(static_cast<T>(0)),
                                              m10(static_cast<T>(0)), m11(static_cast<T>(scalar)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<2, 2, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<3, 3, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<4, 4, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(U m00, U m01, U m10, U m11) : m00(static_cast<T>(m00)), m01(static_cast<T>(m01)),
                                                                m10(static_cast<T>(m10)), m11(static_cast<T>(m11)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const Vector::vec<2, U> v0, Vector::vec<2, U> v1) : m00(static_cast<T>(v0.x)), m01(static_cast<T>(v0.y)),
                                                                                      m10(static_cast<T>(v1.x)), m11(static_cast<T>(v1.y)) {}

    // --initialization list constructors-- //
    MATH_INLINE mat(std::initializer_list<T> list)
    {
        if (list.size() < 4)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("mat2x2", list.size()));
        }
        auto it = list.begin();
        m00 = *it++;
        m01 = *it++;
        m10 = *it++;
        m11 = *it;
    }

    MATH_INLINE mat(std::initializer_list<Vector::vec<2, T>> list)
    {
        if (list.size() < 2)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("mat2x2", list.size()));
        }
        auto it = list.begin();
        c[0] = *it++;
        c[1] = *it;
    }

    // --unary arithmetic operators-- //
    MATH_INLINE mat operator+() const
    {
        return *this;
    }

    MATH_INLINE mat operator-() const
    {
        return mat(-m00, -m01, -m10, -m11);
    }

    template <typename U>
    MATH_INLINE mat &operator+=(U scalar)
    {
        m00 += static_cast<T>(scalar);
        m01 += static_cast<T>(scalar);
        m10 += static_cast<T>(scalar);
        m11 += static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator+=(const mat<2, 2, U> &m)
    {
        m00 += static_cast<T>(m.m00);
        m01 += static_cast<T>(m.m01);
        m10 += static_cast<T>(m.m10);
        m11 += static_cast<T>(m.m11);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator-=(U scalar)
    {
        m00 -= static_cast<T>(scalar);
        m01 -= static_cast<T>(scalar);
        m10 -= static_cast<T>(scalar);
        m11 -= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator-=(const mat<2, 2, U> &m)
    {
        m00 -= static_cast<T>(m.m00);
        m01 -= static_cast<T>(m.m01);
        m10 -= static_cast<T>(m.m10);
        m11 -= static_cast<T>(m.m11);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator*=(U scalar)
    {
        m00 *= static_cast<T>(scalar);
        m01 *= static_cast<T>(scalar);
        m10 *= static_cast<T>(scalar);
        m11 *= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator*=(const mat<2, 2, U> &m)
    {
        m00 *= static_cast<T>(m.m00);
        m01 *= static_cast<T>(m.m01);
        m10 *= static_cast<T>(m.m10);
        m11 *= static_cast<T>(m.m11);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator/=(U scalar)
    {
        m00 /= static_cast<T>(scalar);
        m01 /= static_cast<T>(scalar);
        m10 /= static_cast<T>(scalar);
        m11 /= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator/=(const mat<2, 2, U> &m)
    {
        m00 /= static_cast<T>(m.m00);
        m01 /= static_cast<T>(m.m01);
        m10 /= static_cast<T>(m.m10);
        m11 /= static_cast<T>(m.m11);
        return *this;
    }

    // --increment and decrement operators-- //
    MATH_INLINE mat &operator++()
    {
        static_assert(std::is_integral<T>::value, "mat<T>::operator++(): T must be an integral type.");

        ++m00;
        ++m01;
        ++m10;
        ++m11;
        return *this;
    }

    MATH_INLINE mat operator++(int)
    {
        static_assert(std::is_integral<T>::value, "mat<T>::operator++(int): T must be an integral type.");

        mat temp(*this);
        ++*this;
        return temp;
    }

    MATH_INLINE mat &operator--()
    {
        static_assert(std::is_integral<T>::value, "mat2x2<T>::operator--(): T must be an integral type.");

        --m00;
        --m01;
        --m10;
        --m11;
        return *this;
    }

    MATH_INLINE mat operator--(int)
    {
        static_assert(std::is_integral<T>::value, "mat2x2<T>::operator--(int): T must be an integral type.");

        mat temp(*this);
        --*this;
        return temp;
    }

    // --binary arithmetic operators-- //
    friend MATH_INLINE mat operator+(const mat &m, T scalar)
    {
        return {m.m00 + scalar, m.m01 + scalar, m.m10 + scalar, m.m11 + scalar};
    }

    friend MATH_INLINE mat operator+(T scalar, const mat &m)
    {
        return {scalar + m.m00, scalar + m.m01, scalar + m.m10, scalar + m.m11};
    }

    friend MATH_INLINE mat operator+(const mat &m1, const mat &m2)
    {
        return {m1.m00 + m2.m00, m1.m01 + m2.m01, m1.m10 + m2.m10, m1.m11 + m2.m11};
    }

    friend MATH_INLINE mat operator-(const mat &m, T scalar)
    {
        return {m.m00 - scalar, m.m01 - scalar, m.m10 - scalar, m.m11 - scalar};
    }

    friend MATH_INLINE mat operator-(T scalar, const mat &m)
    {
        return {scalar - m.m00, scalar - m.m01, scalar - m.m10, scalar - m.m11};
    }

    friend MATH_INLINE mat operator-(const mat &m1, const mat &m2)
    {
        return {m1.m00 - m2.m00, m1.m01 - m2.m01, m1.m10 - m2.m10, m1.m11 - m2.m11};
    }

    friend MATH_INLINE mat operator*(const mat &m, T scalar)
    {
        return {m.m00 * scalar, m.m01 * scalar, m.m10 * scalar, m.m11 * scalar};
    }

    friend MATH_INLINE mat operator*(T scalar, const mat &m)
    {
        return {scalar * m.m00, scalar * m.m01, scalar * m.m10, scalar * m.m11};
    }

    friend MATH_INLINE mat operator*(const mat &m1, const mat &m2)
    {
        return mat(m1.m00 * m2.m00, m1.m01 * m2.m01, m1.m10 * m2.m10, m1.m11 * m2.m11);
    }

    friend MATH_INLINE mat operator/(const mat &m, T scalar)
    {
        return mat(m.m00 / scalar, m.m01 / scalar, m.m10 / scalar, m.m11 / scalar);
    }

    friend MATH_INLINE mat operator/(T scalar, const mat &m)
    {
        return mat(scalar / m.m00, scalar / m.m01, scalar / m.m10, scalar / m.m11);
    }

    friend MATH_INLINE mat operator/(const mat &m1, const mat &m2)
    {
        return mat(m1.m00 / m2.m00, m1.m01 / m2.m01, m1.m10 / m2.m10, m1.m11 / m2.m11);
    }

    // --comparison operators-- //
    friend MATH_INLINE bool operator==(const mat &m1, const mat &m2)
    {
        return m1.m00 == m2.m00 && m1.m01 == m2.m01 && m1.m10 == m2.m10 && m1.m11 == m2.m11;
    }

    friend MATH_INLINE bool operator!=(const mat &m1, const mat &m2)
    {
        return m1.m00 != m2.m00 || m1.m01 != m2.m01 || m1.m10 != m2.m10 || m1.m11 != m2.m11;
    }
};

// --stream operators-- //
#ifdef MATH_IOS
template <typename T>
std::ostream &operator<<(std::ostream &os, const mat<2, 2, T> &m)
{
    os << "mat<" << typeid(T).name() << ">{" << '\n'
       << m.m00 << ", " << m.m10 << ", " << '\n'
       << m.m01 << ", " << m.m11 << "}";
    return os;
}

template <typename T>
std::string to_string(const mat<2, 2, T> &m)
{
    return "mat<" + std::string(typeid(T).name()) + ">{" + '\n' +
           std::to_string(m.m00) + ", " + std::to_string(m.m10) + ", " + '\n' +
           std::to_string(m.m01) + ", " + std::to_string(m.m11) + "}";
}
#endif

#ifdef MATH_TEMPLATE_ALIASES
using mat2x2i = mat<2, 2, int>;
using mat2x2u = mat<2, 2, unsigned int>;
using mat2x2f = mat<2, 2, float>;
using mat2x2d = mat<2, 2, double>;
#endif

MATRIX_NAMESPACE_END
MATH_NAMESPACE_END