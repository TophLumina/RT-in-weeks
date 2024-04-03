#pragma once

#include "../numeric/Numeric.hpp"
#include "../numeric/Random.hpp"

MATH_NAMESPACE_BEGIN
MATRIX_NAMESPACE_BEGIN

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> abs(mat<M, N, T> const &m)
{
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = Math::abs(m[i][j]);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> random_range(mat<M, N, T> const &min, mat<M, N, T> const &max)
{
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = Math::random_range(min[i][j], max[i][j]);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> min(mat<M, N, T> const &m, mat<M, N, T> const &n)
{
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = Math::min(m[i][j], n[i][j]);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> max(mat<M, N, T> const &m, mat<M, N, T> const &n)
{
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = Math::max(m[i][j], n[i][j]);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> clamp(mat<M, N, T> const &m, mat<M, N, T> const &min, mat<M, N, T> const &max)
{
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = Math::clamp(m[i][j], min[i][j], max[i][j]);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> saturate(mat<M, N, T> const &m)
{
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = Math::saturate(m[i][j]);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> linear_lerp(mat<M, N, T> const &m, mat<M, N, T> const &n, mat<M, N, T> const &a)
{
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = Math::linear_lerp(m[i][j], n[i][j], a[i][j]);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<N, M, T> transpose(mat<M, N, T> const &m)
{
    mat<N, M, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[j][i] = m[i][j];
        }
    }
    return result;
}

// forward declaration
template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS T determinant(mat<M, N, T> const &m);

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS T cofactor(mat<M, N, T> const &m, LENGTH_TYPE row, LENGTH_TYPE col)
{
    mat<M - 1, N - 1, T> subMatrix;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            if (i != row && j != col)
            {
                LENGTH_TYPE sub_i = i < row ? i : i - 1;
                LENGTH_TYPE sub_j = j < col ? j : j - 1;
                subMatrix[sub_i][sub_j] = m[i][j];
            }
        }
    }
    T sign = ((row + col) % 2 == 0) ? static_cast<T>(1) : static_cast<T>(-1);
    return sign * determinant(subMatrix);
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> minor(mat<M, N, T> const &m, LENGTH_TYPE row, LENGTH_TYPE col)
{
    mat<M - 1, N - 1, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            if (i != row && j != col)
            {
                LENGTH_TYPE sub_i = i < row ? i : i - 1;
                LENGTH_TYPE sub_j = j < col ? j : j - 1;
                result[sub_i][sub_j] = m[i][j];
            }
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS T determinant(mat<M, N, T> const &m)
{
    static_assert(M == N, "determinant is only defined for square matrices");
    T result = 0;
    if constexpr (M == 2)
    {
        result = m[0][0] * m[1][1] - m[0][1] * m[1][0];
    }
    else
    {
        for (LENGTH_TYPE i = 0; i < M; ++i)
        {
            result += m[i][0] * cofactor(m, i, 0);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS T trace(mat<M, N, T> const &m)
{
    static_assert(M == N, "trace is only defined for square matrices");
    T result = 0;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        result += m[i][i];
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> adjugate(mat<M, N, T> const &m)
{
    static_assert(M == N, "adjugate is only defined for square matrices");
    mat<M, N, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i][j] = determinant(minor(m, j, i)) * ((i + j) % 2 == 0 ? 1 : -1);
        }
    }
    return result;
}

template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS mat<M, N, T> inverse(mat<M, N, T> const &m)
{
    static_assert(M == N, "inverse is only defined for square matrices");
    T const det = determinant(m);
    if (det == 0)
    {
        throw std::runtime_error("matrix is not invertible");
    }
    return adjugate(m) / det;
}

// TODO:: extend this to support non-square matrices
// Only Support Squared Matrices
template <LENGTH_TYPE M, LENGTH_TYPE N, typename T>
static MATH_FUNCTION_QUALIFIERS Vector::vec<M, T> operator*(mat<M, N, T> const &m, Vector::vec<N, T> const &v)
{
    Vector::vec<M, T> result;
    for (LENGTH_TYPE i = 0; i < M; ++i)
    {
        for (LENGTH_TYPE j = 0; j < N; ++j)
        {
            result[i] += m[j][i] * v[j];
        }
    }
    return result;
}

MATRIX_NAMESPACE_END
MATH_NAMESPACE_END