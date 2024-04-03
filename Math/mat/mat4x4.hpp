#pragma once

#include "../Config.h"
#include "../structures.h"

#include <initializer_list>

MATH_NAMESPACE_BEGIN
MATRIX_NAMESPACE_BEGIN

template <typename T>
struct mat<4, 4, T>
{
    union
    {
        Vector::vec<4, T> c[4];
        T data[16];
        struct
        {
            // Stored in column major order
            // source matrix:
            // | m00 m10 m20 m30 |
            // | m01 m11 m21 m31 |
            // | m02 m12 m22 m32 |
            // | m03 m13 m23 m33 |
            T m00, m01, m02, m03;
            T m10, m11, m12, m13;
            T m20, m21, m22, m23;
            T m30, m31, m32, m33;
        };
    };

    // --accessors-- //
    MATH_INLINE LENGTH_TYPE size() const
    {
        return 4;
    }

    MATH_INLINE Vector::vec<4, T> &at(LENGTH_TYPE i)
    {
        if (i < 0 || i >= 4)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("mat4x4"));
        }
        return c[i];
    }

    MATH_INLINE Vector::vec<4, T> const &at(LENGTH_TYPE i) const
    {
        if (i < 0 || i >= 4)
        {
            throw std::out_of_range(OUT_OF_RANGE_MSG("mat4x4"));
        }
        return c[i];
    }

    // operator[] will not have out of range checks for performance
    MATH_INLINE Vector::vec<4, T> &operator[](LENGTH_TYPE i)
    {
        return c[i];
    }

    MATH_INLINE Vector::vec<4, T> const &operator[](LENGTH_TYPE i) const
    {
        return c[i];
    }
    
    // --implicit basic constructors-- //
    MATH_INLINE mat() : m00(0), m01(0), m02(0), m03(0),
                        m10(0), m11(0), m12(0), m13(0),
                        m20(0), m21(0), m22(0), m23(0),
                        m30(0), m31(0), m32(0), m33(0) {}
    MATH_INLINE mat(const mat &m) = default;
    MATH_INLINE mat &operator=(const mat &m) = default;
    MATH_INLINE mat(mat &&m) = default;
    MATH_INLINE mat &operator=(mat &&m) = default;

    // --explicit conversion constructors-- //
    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(U scalar) : m00(static_cast<T>(scalar)), m01(static_cast<T>(0)), m02(static_cast<T>(0)), m03(static_cast<T>(0)),
                                              m10(static_cast<T>(0)), m11(static_cast<T>(scalar)), m12(static_cast<T>(0)), m13(static_cast<T>(0)),
                                              m20(static_cast<T>(0)), m21(static_cast<T>(0)), m22(static_cast<T>(scalar)), m23(static_cast<T>(0)),
                                              m30(static_cast<T>(0)), m31(static_cast<T>(0)), m32(static_cast<T>(0)), m33(static_cast<T>(scalar)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<2, 2, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)), m02(0), m03(0),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)), m12(0), m13(0),
                                                           m20(0), m21(0), m22(1), m23(0),
                                                           m30(0), m31(0), m32(0), m33(1) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<3, 3, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)), m02(static_cast<T>(m.m02)), m03(0),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)), m12(static_cast<T>(m.m12)), m13(0),
                                                           m20(static_cast<T>(m.m20)), m21(static_cast<T>(m.m21)), m22(static_cast<T>(m.m22)), m23(0),
                                                           m30(0), m31(0), m32(0), m33(1) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const mat<4, 4, U> &m) : m00(static_cast<T>(m.m00)), m01(static_cast<T>(m.m01)), m02(static_cast<T>(m.m02)), m03(static_cast<T>(m.m03)),
                                                           m10(static_cast<T>(m.m10)), m11(static_cast<T>(m.m11)), m12(static_cast<T>(m.m12)), m13(static_cast<T>(m.m13)),
                                                           m20(static_cast<T>(m.m20)), m21(static_cast<T>(m.m21)), m22(static_cast<T>(m.m22)), m23(static_cast<T>(m.m23)),
                                                           m30(static_cast<T>(m.m30)), m31(static_cast<T>(m.m31)), m32(static_cast<T>(m.m32)), m33(static_cast<T>(m.m33)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(U m00, U m01, U m02, U m03,
                                  U m10, U m11, U m12, U m13,
                                  U m20, U m21, U m22, U m23,
                                  U m30, U m31, U m32, U m33) : m00(static_cast<T>(m00)), m01(static_cast<T>(m01)), m02(static_cast<T>(m02)), m03(static_cast<T>(m03)),
                                                                m10(static_cast<T>(m10)), m11(static_cast<T>(m11)), m12(static_cast<T>(m12)), m13(static_cast<T>(m13)),
                                                                m20(static_cast<T>(m20)), m21(static_cast<T>(m21)), m22(static_cast<T>(m22)), m23(static_cast<T>(m23)),
                                                                m30(static_cast<T>(m30)), m31(static_cast<T>(m31)), m32(static_cast<T>(m32)), m33(static_cast<T>(m33)) {}

    template <typename U>
    MATH_INLINE MATH_EXPLICIT mat(const Vector::vec<4, U> &v0, const Vector::vec<4, U> &v1, const Vector::vec<4, U> &v2, const Vector::vec<4, U> &v3) : c{v0, v1, v2, v3} {}

    // --initializer list constructor-- //
    MATH_INLINE mat(std::initializer_list<T> list)
    {
        if (list.size() != 16)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("mat4x4", list.size()));
        }
        auto it = list.begin();
        m00 = *it++;
        m01 = *it++;
        m02 = *it++;
        m03 = *it++;
        m10 = *it++;
        m11 = *it++;
        m12 = *it++;
        m13 = *it++;
        m20 = *it++;
        m21 = *it++;
        m22 = *it++;
        m23 = *it++;
        m30 = *it++;
        m31 = *it++;
        m32 = *it++;
        m33 = *it;
    }

    MATH_INLINE mat(std::initializer_list<Vector::vec<4, T>> list)
    {
        if (list.size() != 4)
        {
            throw std::invalid_argument(INVALID_INITIALIZER_LIST_ARGS_MSG("vec4x4", list.size()));
        }
        auto it = list.begin();
        c[0] = *it++;
        c[1] = *it++;
        c[2] = *it++;
        c[3] = *it;
    }

    // --unary arithmetic operators-- //
    MATH_INLINE mat operator+() const
    {
        return *this;
    }

    MATH_INLINE mat operator-() const
    {
        return mat(-m00, -m01, -m02, -m03,
                   -m10, -m11, -m12, -m13,
                   -m20, -m21, -m22, -m23,
                   -m30, -m31, -m32, -m33);
    }

    template <typename U>
    MATH_INLINE mat &operator+=(U scalar)
    {
        m00 += scalar;
        m01 += scalar;
        m02 += scalar;
        m03 += scalar;
        m10 += scalar;
        m11 += scalar;
        m12 += scalar;
        m13 += scalar;
        m20 += scalar;
        m21 += scalar;
        m22 += scalar;
        m23 += scalar;
        m30 += scalar;
        m31 += scalar;
        m32 += scalar;
        m33 += scalar;
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator+=(const mat<4, 4, U> &m)
    {
        m00 += m.m00;
        m01 += m.m01;
        m02 += m.m02;
        m03 += m.m03;
        m10 += m.m10;
        m11 += m.m11;
        m12 += m.m12;
        m13 += m.m13;
        m20 += m.m20;
        m21 += m.m21;
        m22 += m.m22;
        m23 += m.m23;
        m30 += m.m30;
        m31 += m.m31;
        m32 += m.m32;
        m33 += m.m33;
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator-=(U scalar)
    {
        m00 -= scalar;
        m01 -= scalar;
        m02 -= scalar;
        m03 -= scalar;
        m10 -= scalar;
        m11 -= scalar;
        m12 -= scalar;
        m13 -= scalar;
        m20 -= scalar;
        m21 -= scalar;
        m22 -= scalar;
        m23 -= scalar;
        m30 -= scalar;
        m31 -= scalar;
        m32 -= scalar;
        m33 -= scalar;
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator-=(const mat<4, 4, U> &m)
    {
        m00 -= m.m00;
        m01 -= m.m01;
        m02 -= m.m02;
        m03 -= m.m03;
        m10 -= m.m10;
        m11 -= m.m11;
        m12 -= m.m12;
        m13 -= m.m13;
        m20 -= m.m20;
        m21 -= m.m21;
        m22 -= m.m22;
        m23 -= m.m23;
        m30 -= m.m30;
        m31 -= m.m31;
        m32 -= m.m32;
        m33 -= m.m33;
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator*=(U scalar)
    {
        m00 *= scalar;
        m01 *= scalar;
        m02 *= scalar;
        m03 *= scalar;
        m10 *= scalar;
        m11 *= scalar;
        m12 *= scalar;
        m13 *= scalar;
        m20 *= scalar;
        m21 *= scalar;
        m22 *= scalar;
        m23 *= scalar;
        m30 *= scalar;
        m31 *= scalar;
        m32 *= scalar;
        m33 *= scalar;
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator*=(const mat<4, 4, U> &m)
    {
        m00 *= m.m00;
        m01 *= m.m01;
        m02 *= m.m02;
        m03 *= m.m03;
        m10 *= m.m10;
        m11 *= m.m11;
        m12 *= m.m12;
        m13 *= m.m13;
        m20 *= m.m20;
        m21 *= m.m21;
        m22 *= m.m22;
        m23 *= m.m23;
        m30 *= m.m30;
        m31 *= m.m31;
        m32 *= m.m32;
        m33 *= m.m33;
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator/=(U scalar)
    {
        m00 /= scalar;
        m01 /= scalar;
        m02 /= scalar;
        m03 /= scalar;
        m10 /= scalar;
        m11 /= scalar;
        m12 /= scalar;
        m13 /= scalar;
        m20 /= scalar;
        m21 /= scalar;
        m22 /= scalar;
        m23 /= scalar;
        m30 /= scalar;
        m31 /= scalar;
        m32 /= scalar;
        m33 /= scalar;
        return *this;
    }

    template <typename U>
    MATH_INLINE mat &operator/=(const mat<4, 4, U> &m)
    {
        m00 /= m.m00;
        m01 /= m.m01;
        m02 /= m.m02;
        m03 /= m.m03;
        m10 /= m.m10;
        m11 /= m.m11;
        m12 /= m.m12;
        m13 /= m.m13;
        m20 /= m.m20;
        m21 /= m.m21;
        m22 /= m.m22;
        m23 /= m.m23;
        m30 /= m.m30;
        m31 /= m.m31;
        m32 /= m.m32;
        m33 /= m.m33;
        return *this;
    }

    // --increment and decrement operators-- //
    MATH_INLINE mat &operator++()
    {
        static_assert(std::is_integral<T>::value, "mat<T>::operator++(): T must be an integral type.");

        ++m00;
        ++m01;
        ++m02;
        ++m03;
        ++m10;
        ++m11;
        ++m12;
        ++m13;
        ++m20;
        ++m21;
        ++m22;
        ++m23;
        ++m30;
        ++m31;
        ++m32;
        ++m33;
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
        --m03;
        --m10;
        --m11;
        --m12;
        --m13;
        --m20;
        --m21;
        --m22;
        --m23;
        --m30;
        --m31;
        --m32;
        --m33;
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
        return {m.m00 + scalar, m.m01 + scalar, m.m02 + scalar, m.m03 + scalar,
                m.m10 + scalar, m.m11 + scalar, m.m12 + scalar, m.m13 + scalar,
                m.m20 + scalar, m.m21 + scalar, m.m22 + scalar, m.m23 + scalar,
                m.m30 + scalar, m.m31 + scalar, m.m32 + scalar, m.m33 + scalar};
    }

    friend MATH_INLINE mat operator+(T scalar, const mat &m)
    {
        return {m.m00 + scalar, m.m01 + scalar, m.m02 + scalar, m.m03 + scalar,
                m.m10 + scalar, m.m11 + scalar, m.m12 + scalar, m.m13 + scalar,
                m.m20 + scalar, m.m21 + scalar, m.m22 + scalar, m.m23 + scalar,
                m.m30 + scalar, m.m31 + scalar, m.m32 + scalar, m.m33 + scalar};
    }

    friend MATH_INLINE mat operator+(const mat &m1, const mat &m2)
    {
        return {m1.m00 + m2.m00, m1.m01 + m2.m01, m1.m02 + m2.m02, m1.m03 + m2.m03,
                m1.m10 + m2.m10, m1.m11 + m2.m11, m1.m12 + m2.m12, m1.m13 + m2.m13,
                m1.m20 + m2.m20, m1.m21 + m2.m21, m1.m22 + m2.m22, m1.m23 + m2.m23,
                m1.m30 + m2.m30, m1.m31 + m2.m31, m1.m32 + m2.m32, m1.m33 + m2.m33};
    }

    friend MATH_INLINE mat operator-(const mat &m, T scalar)
    {
        return {m.m00 - scalar, m.m01 - scalar, m.m02 - scalar, m.m03 - scalar,
                m.m10 - scalar, m.m11 - scalar, m.m12 - scalar, m.m13 - scalar,
                m.m20 - scalar, m.m21 - scalar, m.m22 - scalar, m.m23 - scalar,
                m.m30 - scalar, m.m31 - scalar, m.m32 - scalar, m.m33 - scalar};
    }

    friend MATH_INLINE mat operator-(T scalar, const mat &m)
    {
        return {scalar - m.m00, scalar - m.m01, scalar - m.m02, scalar - m.m03,
                scalar - m.m10, scalar - m.m11, scalar - m.m12, scalar - m.m13,
                scalar - m.m20, scalar - m.m21, scalar - m.m22, scalar - m.m23,
                scalar - m.m30, scalar - m.m31, scalar - m.m32, scalar - m.m33};
    }

    friend MATH_INLINE mat operator-(const mat &m1, const mat &m2)
    {
        return {m1.m00 - m2.m00, m1.m01 - m2.m01, m1.m02 - m2.m02, m1.m03 - m2.m03,
                m1.m10 - m2.m10, m1.m11 - m2.m11, m1.m12 - m2.m12, m1.m13 - m2.m13,
                m1.m20 - m2.m20, m1.m21 - m2.m21, m1.m22 - m2.m22, m1.m23 - m2.m23,
                m1.m30 - m2.m30, m1.m31 - m2.m31, m1.m32 - m2.m32, m1.m33 - m2.m33};
    }

    friend MATH_INLINE mat operator*(const mat &m, T scalar)
    {
        return {m.m00 * scalar, m.m01 * scalar, m.m02 * scalar, m.m03 * scalar,
                m.m10 * scalar, m.m11 * scalar, m.m12 * scalar, m.m13 * scalar,
                m.m20 * scalar, m.m21 * scalar, m.m22 * scalar, m.m23 * scalar,
                m.m30 * scalar, m.m31 * scalar, m.m32 * scalar, m.m33 * scalar};
    }

    friend MATH_INLINE mat operator*(T scalar, const mat &m)
    {
        return {m.m00 * scalar, m.m01 * scalar, m.m02 * scalar, m.m03 * scalar,
                m.m10 * scalar, m.m11 * scalar, m.m12 * scalar, m.m13 * scalar,
                m.m20 * scalar, m.m21 * scalar, m.m22 * scalar, m.m23 * scalar,
                m.m30 * scalar, m.m31 * scalar, m.m32 * scalar, m.m33 * scalar};
    }

    friend MATH_INLINE mat operator*(const mat &m1, const mat &m2)
    {
        return {m1.m00 * m2.m00, m1.m01 * m2.m01, m1.m02 * m2.m02, m1.m03 * m2.m03,
                m1.m10 * m2.m10, m1.m11 * m2.m11, m1.m12 * m2.m12, m1.m13 * m2.m13,
                m1.m20 * m2.m20, m1.m21 * m2.m21, m1.m22 * m2.m22, m1.m23 * m2.m23,
                m1.m30 * m2.m30, m1.m31 * m2.m31, m1.m32 * m2.m32, m1.m33 * m2.m33};
    }

    friend MATH_INLINE mat operator/(const mat &m, T scalar)
    {
        return {m.m00 / scalar, m.m01 / scalar, m.m02 / scalar, m.m03 / scalar,
                m.m10 / scalar, m.m11 / scalar, m.m12 / scalar, m.m13 / scalar,
                m.m20 / scalar, m.m21 / scalar, m.m22 / scalar, m.m23 / scalar,
                m.m30 / scalar, m.m31 / scalar, m.m32 / scalar, m.m33 / scalar};
    }

    friend MATH_INLINE mat operator/(T scalar, const mat &m)
    {
        return {scalar / m.m00, scalar / m.m01, scalar / m.m02, scalar / m.m03,
                scalar / m.m10, scalar / m.m11, scalar / m.m12, scalar / m.m13,
                scalar / m.m20, scalar / m.m21, scalar / m.m22, scalar / m.m23,
                scalar / m.m30, scalar / m.m31, scalar / m.m32, scalar / m.m33};
    }

    friend MATH_INLINE mat operator/(const mat &m1, const mat &m2)
    {
        return {m1.m00 / m2.m00, m1.m01 / m2.m01, m1.m02 / m2.m02, m1.m03 / m2.m03,
                m1.m10 / m2.m10, m1.m11 / m2.m11, m1.m12 / m2.m12, m1.m13 / m2.m13,
                m1.m20 / m2.m20, m1.m21 / m2.m21, m1.m22 / m2.m22, m1.m23 / m2.m23,
                m1.m30 / m2.m30, m1.m31 / m2.m31, m1.m32 / m2.m32, m1.m33 / m2.m33};
    }

    // --comparison operators-- //
    friend MATH_INLINE bool operator==(const mat &m1, const mat &m2)
    {
        return m1.m00 == m2.m00 && m1.m01 == m2.m01 && m1.m02 == m2.m02 && m1.m03 == m2.m03 &&
               m1.m10 == m2.m10 && m1.m11 == m2.m11 && m1.m12 == m2.m12 && m1.m13 == m2.m13 &&
               m1.m20 == m2.m20 && m1.m21 == m2.m21 && m1.m22 == m2.m22 && m1.m23 == m2.m23 &&
               m1.m30 == m2.m30 && m1.m31 == m2.m31 && m1.m32 == m2.m32 && m1.m33 == m2.m33;
    }

    friend MATH_INLINE bool operator!=(const mat &m1, const mat &m2)
    {
        return m1.m00 != m2.m00 || m1.m01 != m2.m01 || m1.m02 != m2.m02 || m1.m03 != m2.m03 ||
               m1.m10 != m2.m10 || m1.m11 != m2.m11 || m1.m12 != m2.m12 || m1.m13 != m2.m13 ||
               m1.m20 != m2.m20 || m1.m21 != m2.m21 || m1.m22 != m2.m22 || m1.m23 != m2.m23 ||
               m1.m30 != m2.m30 || m1.m31 != m2.m31 || m1.m32 != m2.m32 || m1.m33 != m2.m33;
    }
};

// --stream operators-- //
#ifdef MATH_IOS
template <typename T>
std::ostream &operator<<(std::ostream &os, const mat<4, 4, T> &m)
{
    os << "mat<" << typeid(T).name() << ">{" << '\n'
       << m.m00 << ", " << m.m01 << ", " << m.m02 << ", " << m.m03 << ", " << '\n'
       << m.m10 << ", " << m.m11 << ", " << m.m12 << ", " << m.m13 << ", " << '\n'
       << m.m20 << ", " << m.m21 << ", " << m.m22 << ", " << m.m23 << ", " << '\n'
       << m.m30 << ", " << m.m31 << ", " << m.m32 << ", " << m.m33 << "}";
    return os;
}

template <typename T>
std::string to_string(const mat<4, 4, T> &m)
{
    std::string str;
    str += "mat<" + std::string(typeid(T).name()) + ">{" + '\n';
    str += std::to_string(m.m00) + ", " + std::to_string(m.m01) + ", " + std::to_string(m.m02) + ", " + std::to_string(m.m03) + ", " + '\n';
    str += std::to_string(m.m10) + ", " + std::to_string(m.m11) + ", " + std::to_string(m.m12) + ", " + std::to_string(m.m13) + ", " + '\n';
    str += std::to_string(m.m20) + ", " + std::to_string(m.m21) + ", " + std::to_string(m.m22) + ", " + std::to_string(m.m23) + ", " + '\n';
    str += std::to_string(m.m30) + ", " + std::to_string(m.m31) + ", " + std::to_string(m.m32) + ", " + std::to_string(m.m33) + "}";
    return str;
}
#endif

#ifdef MATH_TEMPLATE_ALIASES
using mat4x4i = mat<4, 4, int>;
using mat4x4u = mat<4, 4, unsigned int>;
using mat4x4f = mat<4, 4, float>;
using mat4x4d = mat<4, 4, double>;
#endif

MATRIX_NAMESPACE_END
MATH_NAMESPACE_END