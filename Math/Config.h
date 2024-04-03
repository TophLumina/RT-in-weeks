#pragma once

#define MATH_NAMESPACE_BEGIN namespace Math {
#define MATH_NAMESPACE_END }
#define VECTOR_NAMESPACE_BEGIN namespace Vector {
#define VECTOR_NAMESPACE_END }
#define MATRIX_NAMESPACE_BEGIN namespace Matrix {
#define MATRIX_NAMESPACE_END } 

#ifdef MATH_SUPPORT_CONSTEXPR
    #define MATH_CONSTEXPR constexpr
#else
    #define MATH_CONSTEXPR
#endif

#ifdef MATH_FORCE_EXPLICIT
    #define MATH_EXPLICIT explicit
#else
    #define MATH_EXPLICIT
#endif

// FUNCS IN HEADERS ARE INLINE BY DEFAULT
#ifdef MATH_FORCE_INLINE
    #define MATH_INLINE inline
#else
    #define MATH_INLINE
#endif

#define MATH_FUNCTION_QUALIFIERS MATH_CONSTEXPR MATH_INLINE