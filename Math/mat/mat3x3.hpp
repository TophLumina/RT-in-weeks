#pragma once

#include "../Config.h"
#include "../structures.h"

#include <initializer_list>

MATH_NAMESPACE_BEGIN
MATRIX_NAMESPACE_BEGIN

template <typename T>
struct mat<3, 3, T>
{
    union
    {
        Vector::vec<3, T> c[3];
        T data[9];
        struct
        {
            // Stored in column major order
            // source matrix:
            // | m00 m10 m20 |
            // | m01 m11 m21 |
            // | m02 m12 m22 |
            T m00, m01, m02;
            T m10, m11, m12;
            T m20, m21, m22;
        };
    };

    // --accessors-- //
    MATH_INLINE LENGTH_TYPE size() const
    {
        return 3;
    }

    MATH_INLINE Vector::vec<3, T> &at(LENGTH_TYPE i)
    {
        if (i < 0 || i >= 3)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("mat3x3"));
        }
        return c[i];
    }

    MATH_INLINE const Vector::vec<3, T> &at(LENGTH_TYPE i) const
    {
        if (i < 0 || i >= 3)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("mat3x3"));
        }
        return c[i];
    }

    // operator[] will not have out of range checks for performance
    MATH_INLINE Vector::vec<3, T> &operator[](LENGTH_TYPE i)
    {
        return c[i];
    }

    MATH_INLINE Vector::vec<3, T> const &operator[](LENGTH_TYPE i) const
    {
        return c[i];
    }
    
    // --implicit basic constructors-- //
    MATH_INLINE mat() : m00(0), m01(0), m02(0),
                        m10(0), m11(0), m12(0),
                        m20(0), m21(0), m22(0) {}
    MATH_INLINE mat(const mat &m) = default;
    MATH_INLINE mat &operator=(const mat &m) = default;
    MATH_INLINE mat(mat &&m) = default;
    MATH_INLINE mat &operator=(mat &&m) = default;

    // --explicit conversion constructors-- //
    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(U scalar) : m00(static_cast<T>(scalar)), m01(static_cast<T>(0)), m02(static_cast<T>(0)),
                                              m10(static_cast<T>(0)), m11(static_cast<T>(scalar)), m12(static_cast<T>(0)),
                                              m20(static_cast<T>(0)), m21(static_cast<T>(0)), m22(static_cast<T>(scalar)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<2, 2, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)), m02(static_cast<T>(0)),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)), m12(static_cast<T>(0)),
                                                           m20(static_cast<T>(0)), m21(static_cast<T>(0)), m22(static_cast<T>(1)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<3, 3, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)), m02(static_cast<T>(m.m02)),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)), m12(static_cast<T>(m.m12)),
                                                           m20(static_cast<T>(m.m20)), m21(static_cast<T>(m.m21)), m22(static_cast<T>(m.m22)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<4, 4, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)), m02(static_cast<T>(m.m02)),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)), m12(static_cast<T>(m.m12)),
                                                           m20(static_cast<T>(m.m20)), m21(static_cast<T>(m.m21)), m22(static_cast<T>(m.m22)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(U m00, U m01, U m02, U m10, U m11, U m12, U m20, U m21, U m22) : m00(static_cast<T>(m00)), m01(static_cast<T>(m01)), m02(static_cast<T>(m02)),
                                                                                                   m10(static_cast<T>(m10)), m11(static_cast<T>(m11)), m12(static_cast<T>(m12)),
                                                                                                   m20(static_cast<T>(m20)), m21(static_cast<T>(m21)), m22(static_cast<T>(m22)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const Vector::vec<3, U> &v0, const Vector::vec<3, U> &v1, const Vector::vec<3, U> &v2) : m00(static_cast<T>(v0.x)), m01(static_cast<T>(v0.y)), m02(static_cast<T>(v0.z)),
                                                                                                                           m10(static_cast<T>(v1.x)), m11(static_cast<T>(v1.y)), m12(static_cast<T>(v1.z)),
                                                                                                                           m20(static_cast<T>(v2.x)), m21(static_cast<T>(v2.y)), m22(static_cast<T>(v2.z)) {}

    // --initializer list constructor-- //
    MATH_INLINE mat(std::initializer_list<T> list)
    {
        if (list.size() != 9)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("mat3x3", list.size()));
        }
        auto it = list.begin();
        m00 = *it++;
        m01 = *it++;
        m02 = *it++;
        m10 = *it++;
        m11 = *it++;
        m12 = *it++;
        m20 = *it++;
        m21 = *it++;
        m22 = *it;
    }

    MATH_INLINE mat(std::initializer_list<Vector::vec<3, T>> list)
    {
        if (list.size() != 3)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("mat3x3", list.size()));
        }
        auto it = list.begin();
        c[0] = *it++;
        c[1] = *it++;
        c[2] = *it;
    }

    // --unary arithmetic operators-- //
    MATH_INLINE mat operator+() const
    {
        return *this;
    }

    MATH_INLINE mat operator-() const
    {
        return {-m00, -m01, -m02, -m10, -m11, -m12, -m20, -m21, -m22};
    }

    template <typename U>
    MATH_INLINE mat &operator+=(U scalar)
    {
        m00 += static_cast<T>(scalar);
        m01 += static_cast<T>(scalar);
        m02 += static_cast<T>(scalar);
        m10 += static_cast<T>(scalar);
        m11 += static_cast<T>(scalar);
        m12 += static_cast<T>(scalar);
        m20 += static_cast<T>(scalar);
        m21 += static_cast<T>(scalar);
        m22 += static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator+=(const mat<3, 3, U> &m)
    {
        m00 += static_cast<T>(m.m00);
        m01 += static_cast<T>(m.m01);
        m02 += static_cast<T>(m.m02);
        m10 += static_cast<T>(m.m10);
        m11 += static_cast<T>(m.m11);
        m12 += static_cast<T>(m.m12);
        m20 += static_cast<T>(m.m20);
        m21 += static_cast<T>(m.m21);
        m22 += static_cast<T>(m.m22);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator-=(U scalar)
    {
        m00 -= static_cast<T>(scalar);
        m01 -= static_cast<T>(scalar);
        m02 -= static_cast<T>(scalar);
        m10 -= static_cast<T>(scalar);
        m11 -= static_cast<T>(scalar);
        m12 -= static_cast<T>(scalar);
        m20 -= static_cast<T>(scalar);
        m21 -= static_cast<T>(scalar);
        m22 -= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator-=(const mat<3, 3, U> &m)
    {
        m00 -= static_cast<T>(m.m00);
        m01 -= static_cast<T>(m.m01);
        m02 -= static_cast<T>(m.m02);
        m10 -= static_cast<T>(m.m10);
        m11 -= static_cast<T>(m.m11);
        m12 -= static_cast<T>(m.m12);
        m20 -= static_cast<T>(m.m20);
        m21 -= static_cast<T>(m.m21);
        m22 -= static_cast<T>(m.m22);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator*=(U scalar)
    {
        m00 *= static_cast<T>(scalar);
        m01 *= static_cast<T>(scalar);
        m02 *= static_cast<T>(scalar);
        m10 *= static_cast<T>(scalar);
        m11 *= static_cast<T>(scalar);
        m12 *= static_cast<T>(scalar);
        m20 *= static_cast<T>(scalar);
        m21 *= static_cast<T>(scalar);
        m22 *= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator*=(const mat<3, 3, U> &m)
    {
        m00 *= static_cast<T>(m.m00);
        m01 *= static_cast<T>(m.m01);
        m02 *= static_cast<T>(m.m02);
        m10 *= static_cast<T>(m.m10);
        m11 *= static_cast<T>(m.m11);
        m12 *= static_cast<T>(m.m12);
        m20 *= static_cast<T>(m.m20);
        m21 *= static_cast<T>(m.m21);
        m22 *= static_cast<T>(m.m22);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator/=(U scalar)
    {
        m00 /= static_cast<T>(scalar);
        m01 /= static_cast<T>(scalar);
        m02 /= static_cast<T>(scalar);
        m10 /= static_cast<T>(scalar);
        m11 /= static_cast<T>(scalar);
        m12 /= static_cast<T>(scalar);
        m20 /= static_cast<T>(scalar);
        m21 /= static_cast<T>(scalar);
        m22 /= static_cast<T>(scalar);
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator/=(const mat<3, 3, U> &m)
    {
        m00 /= static_cast<T>(m.m00);
        m01 /= static_cast<T>(m.m01);
        m02 /= static_cast<T>(m.m02);
        m10 /= static_cast<T>(m.m10);
        m11 /= static_cast<T>(m.m11);
        m12 /= static_cast<T>(m.m12);
        m20 /= static_cast<T>(m.m20);
        m21 /= static_cast<T>(m.m21);
        m22 /= static_cast<T>(m.m22);
        return *this;
    }

    // --increment and decrement operators-- //
    MATH_INLINE mat &operator++()
    {
        static_assert(std::is_integral<T>::value, "mat<T>::operator++(): T must be an integral type.");

        ++m00;
        ++m01;
        ++m02;
        ++m10;
        ++m11;
        ++m12;
        ++m20;
        ++m21;
        ++m22;
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
        static_assert(std::is_integral<T>::value, "mat<T>::operator--(): T must be an integral type.");

        --m00;
        --m01;
        --m02;
        --m10;
        --m11;
        --m12;
        --m20;
        --m21;
        --m22;
        return *this;
    }

    MATH_INLINE mat operator--(int)
    {
        static_assert(std::is_integral<T>::value, "mat<T>::operator--(int): T must be an integral type.");

        mat temp(*this);
        --*this;
        return temp;
    }

    // --binary arithmetic operators-- //
    friend MATH_INLINE mat operator+(const mat &m, T scalar)
    {
        return {m.m00 + scalar, m.m01 + scalar, m.m02 + scalar, m.m10 + scalar, m.m11 + scalar, m.m12 + scalar, m.m20 + scalar, m.m21 + scalar, m.m22 + scalar};
    }

    friend MATH_INLINE mat operator+(T scalar, const mat &m)
    {
        return {scalar + m.m00, scalar + m.m01, scalar + m.m02, scalar + m.m10, scalar + m.m11, scalar + m.m12, scalar + m.m20, scalar + m.m21, scalar + m.m22};
    }

    friend MATH_INLINE mat operator+(const mat &m1, const mat &m2)
    {
        return {m1.m00 + m2.m00, m1.m01 + m2.m01, m1.m02 + m2.m02, m1.m10 + m2.m10, m1.m11 + m2.m11, m1.m12 + m2.m12, m1.m20 + m2.m20, m1.m21 + m2.m21, m1.m22 + m2.m22};
    }

    friend MATH_INLINE mat operator-(const mat &m, T scalar)
    {
        return {m.m00 - scalar, m.m01 - scalar, m.m02 - scalar, m.m10 - scalar, m.m11 - scalar, m.m12 - scalar, m.m20 - scalar, m.m21 - scalar, m.m22 - scalar};
    }

    friend MATH_INLINE mat operator-(T scalar, const mat &m)
    {
        return {scalar - m.m00, scalar - m.m01, scalar - m.m02, scalar - m.m10, scalar - m.m11, scalar - m.m12, scalar - m.m20, scalar - m.m21, scalar - m.m22};
    }

    friend MATH_INLINE mat operator-(const mat &m1, const mat &m2)
    {
        return {m1.m00 - m2.m00, m1.m01 - m2.m01, m1.m02 - m2.m02, m1.m10 - m2.m10, m1.m11 - m2.m11, m1.m12 - m2.m12, m1.m20 - m2.m20, m1.m21 - m2.m21, m1.m22 - m2.m22};
    }

    friend MATH_INLINE mat operator*(const mat &m, T scalar)
    {
        return {m.m00 * scalar, m.m01 * scalar, m.m02 * scalar, m.m10 * scalar, m.m11 * scalar, m.m12 * scalar, m.m20 * scalar, m.m21 * scalar, m.m22 * scalar};
    }

    friend MATH_INLINE mat operator*(T scalar, const mat &m)
    {
        return {scalar * m.m00, scalar * m.m01, scalar * m.m02, scalar * m.m10, scalar * m.m11, scalar * m.m12, scalar * m.m20, scalar * m.m21, scalar * m.m22};
    }

    friend MATH_INLINE mat operator*(const mat &m1, const mat &m2)
    {
        return {m1.m00 * m2.m00, m1.m01 * m2.m01, m1.m02 * m2.m02, m1.m10 * m2.m10, m1.m11 * m2.m11, m1.m12 * m2.m12, m1.m20 * m2.m20, m1.m21 * m2.m21, m1.m22 * m2.m22};
    }

    friend MATH_INLINE mat operator/(const mat &m, T scalar)
    {
        return {m.m00 / scalar, m.m01 / scalar, m.m02 / scalar, m.m10 / scalar, m.m11 / scalar, m.m12 / scalar, m.m20 / scalar, m.m21 / scalar, m.m22 / scalar};
    }

    friend MATH_INLINE mat operator/(T scalar, const mat &m)
    {
        return {scalar / m.m00, scalar / m.m01, scalar / m.m02, scalar / m.m10, scalar / m.m11, scalar / m.m12, scalar / m.m20, scalar / m.m21, scalar / m.m22};
    }

    friend MATH_INLINE mat operator/(const mat &m1, const mat &m2)
    {
        return {m1.m00 / m2.m00, m1.m01 / m2.m01, m1.m02 / m2.m02, m1.m10 / m2.m10, m1.m11 / m2.m11, m1.m12 / m2.m12, m1.m20 / m2.m20, m1.m21 / m2.m21, m1.m22 / m2.m22};
    }

    // --comparison operators-- //
    friend MATH_INLINE bool operator==(const mat &m1, const mat &m2)
    {
        return m1.m00 == m2.m00 && m1.m01 == m2.m01 && m1.m02 == m2.m02 && m1.m10 == m2.m10 && m1.m11 == m2.m11 && m1.m12 == m2.m12 && m1.m20 == m2.m20 && m1.m21 == m2.m21 && m1.m22 == m2.m22;
    }

    friend MATH_INLINE bool operator!=(const mat &m1, const mat &m2)
    {
        return m1.m00 != m2.m00 || m1.m01 != m2.m01 || m1.m02 != m2.m02 || m1.m10 != m2.m10 || m1.m11 != m2.m11 || m1.m12 != m2.m12 || m1.m20 != m2.m20 || m1.m21 != m2.m21 || m1.m22 != m2.m22;
    }
};

// --stream operators-- //
#ifdef MATH_IOS
template <typename T>
std::ostream &operator<<(std::ostream &os, const mat<3, 3, T> &m)
{
    os << "mat<" << typeid(T).name() << ">{" << '\n'
       << m.m00 << ", " << m.m10 << ", " << m.m20 << ", " << '\n'
       << m.m01 << ", " << m.m11 << ", " << m.m21 << ", " << '\n'
       << m.m02 << ", " << m.m12 << ", " << m.m22 << "}";
    return os;
}

template <typename T>
std::string to_string(const mat<3, 3, T> &m)
{
    return "mat<" + std::string(typeid(T).name()) + ">{" + '\n' +
           std::to_string(m.m00) + ", " + std::to_string(m.m10) + ", " + std::to_string(m.m20) + ", " + '\n' +
           std::to_string(m.m01) + ", " + std::to_string(m.m11) + ", " + std::to_string(m.m21) + ", " + '\n' +
           std::to_string(m.m02) + ", " + std::to_string(m.m12) + ", " + std::to_string(m.m22) + "}";
}
#endif

#ifdef MATH_TEMPLATE_ALIASES
using mat3x3i = mat<3, 3, int>;
using mat3x3u = mat<3, 3, unsigned int>;
using mat3x3f = mat<3, 3, float>;
using mat3x3d = mat<3, 3, double>;
#endif

MATRIX_NAMESPACE_END
MATH_NAMESPACE_END