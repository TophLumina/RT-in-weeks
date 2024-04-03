#pragma once

#include "Config.h"

#include <iostream>
#include <typeinfo>
#include <string>

MATH_NAMESPACE_BEGIN

#define LENGTH_TYPE size_t
#define OUT_OF_RANGE_MSG(type_str) (std::string(type_str)) + " -> Index out of range" + '\t' + \
                                       "in FILE -> " + std::string(__FILE__) + '\t' +          \
                                       "at FUNCTION -> " + std::string(__func__) + '\t' +      \
                                       "on LINE -> " + std::to_string(__LINE__)

#define INVALID_INITIALIZER_LIST_ARGS_MSG(type_str, initializer_list_count) (std::string(type_str)) + " -> Invalid initializer_list count" + '\t' + \
                                                                                "in FILE -> " + std::string(__FILE__) + '\t' +                      \
                                                                                "at FUNCTION -> " + std::string(__func__) + '\t' +                  \
                                                                                "on LINE -> " + std::to_string(__LINE__)
VECTOR_NAMESPACE_BEGIN
template <LENGTH_TYPE, typename T>
struct vec;
VECTOR_NAMESPACE_END

MATRIX_NAMESPACE_BEGIN
template <LENGTH_TYPE, LENGTH_TYPE, typename T>
struct mat;
MATRIX_NAMESPACE_END

template <LENGTH_TYPE, typename T>
struct qua;

MATH_NAMESPACE_END