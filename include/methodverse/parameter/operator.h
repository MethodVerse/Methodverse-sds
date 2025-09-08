// operator.h
// This file defines opeators for primitive types and array of primitive types.
// Author: Chenguang Zhao
// Date: 2025-08-29

#pragma once

#include "operation_policy.h"

// ---- Macro to define binary operator functions
#define DEFINE_BINARY_FUNC(FUNC_NAME, OP_TAG)                                    \
template<class T1, class T2>                                                     \
requires (!is_std_vector_v<T1> && !is_std_vector_v<T2> &&                        \
          op_allowed<op_policy<category_t<T1>, category_t<T2>, OP_TAG>, T1, T2>) \
auto FUNC_NAME(const T1& lhs, const T2& rhs) {                                   \
    using policy = op_policy<category_t<T1>, category_t<T2>, OP_TAG>;            \
    using T3 = op_return_t<policy, T1, T2>;                                      \
    return policy::template impl<T1, T2>(lhs, rhs);                              \
}

// ---- Macro to define unary operator functions
#define DEFINE_UNARY_FUNC(FUNC_NAME, OP_TAG)                                               \
template<class T>                                                                          \
requires (op_allowed<op_policy<category_t<T>, void, OP_TAG>, T>)                     \
auto FUNC_NAME(const T& x) {                                                               \
  using policy = op_policy<category_t<T>, void, OP_TAG>;                                   \
  using RetT   = op_return_t<policy, T>;                                             \
  return policy::template impl<T>(x);                                                      \
}

namespace methodverse::parameter {

    DEFINE_BINARY_FUNC(operator+, add_op)
    DEFINE_BINARY_FUNC(operator-, sub_op)
    DEFINE_BINARY_FUNC(operator*, mul_op)
    DEFINE_BINARY_FUNC(operator/, div_op)

    DEFINE_BINARY_FUNC(dot, dot_op)
    DEFINE_BINARY_FUNC(cross, cross_op)
    DEFINE_BINARY_FUNC(coefw_mul, coefw_mul_op)
    DEFINE_BINARY_FUNC(coefw_div, coefw_div_op)

    DEFINE_UNARY_FUNC(transpose, transpose_op)
    DEFINE_UNARY_FUNC(inverse, inverse_op)

    // helpers
    template<class T1, class T2> auto scalar_add_op(const T1& lhs, const T2& rhs) { return lhs + rhs; }

}; // namespace methodverse::parameter

#define DEFINE_VECTOR_BINARY_FUNC(FUNC_NAME, OP_TAG)                          \
template <class T1, class T2, class A1, class A2>                                                 \
auto FUNC_NAME(const std::vector<T1,A1>& lhs, const std::vector<T2,A2>& rhs)        \
requires (op_allowed<op_policy<category_t<T1>, category_t<T2>, OP_TAG>, T1, T2>) \
{                                                                             \
    using policy = op_policy<category_t<T1>, category_t<T2>, OP_TAG>;         \
    using T3 = op_return_t<policy, T1, T2>;                                   \
                                                                              \
    if (lhs.empty() || rhs.empty())                                           \
        throw std::runtime_error("Cannot perform binary operation on empty vectors"); \
                                                                              \
    if (lhs.size() > 1 && rhs.size() > 1 && lhs.size() != rhs.size())         \
        throw std::runtime_error("Vector size mismatch in binary operation"); \
                                                                              \
    if (lhs.size() == 1 && rhs.size() > 1) {                                  \
        std::vector<T3> result(rhs.size());                                   \
        std::transform(rhs.begin(), rhs.end(), result.begin(),                \
                       [&lhs](const T2& v) { return policy::template impl<T1,T2>(lhs[0], v); }); \
        return result;                                                        \
    }                                                                         \
                                                                              \
    if (lhs.size() > 1 && rhs.size() == 1) {                                  \
        std::vector<T3> result(lhs.size());                                   \
        std::transform(lhs.begin(), lhs.end(), result.begin(),                \
                       [&rhs](const T1& v) { return policy::template impl<T1,T2>(v, rhs[0]); }); \
        return result;                                                        \
    }                                                                         \
                                                                              \
    if (lhs.size() == rhs.size()) {                                           \
        std::vector<T3> result(lhs.size());                                   \
        std::transform(lhs.begin(), lhs.end(), rhs.begin(), result.begin(),   \
                       [](const T1& v1, const T2& v2) { return policy::template impl<T1,T2>(v1, v2); }); \
        return result;                                                        \
    }                                                                         \
                                                                              \
    throw std::runtime_error("Unexpected vector size combination in binary operation"); \
}

#define DEFINE_VECTOR_UNARY_FUNC(FUNC_NAME, OP_TAG)                                           \
template <class T>                                                                            \
requires (op_allowed<op_policy<category_t<T>, void, OP_TAG>, T>)                              \
auto FUNC_NAME(const std::vector<T>& a) {                                                     \
    using policy = op_policy<category_t<T>, void, OP_TAG>;                                    \
    using RetT   = op_return_t<policy, T>;                                                    \
                                                                                              \
    std::vector<RetT> result;                                                                 \
    result.reserve(a.size());                                                                 \
    for (const auto& v : a) {                                                                 \
        result.push_back(policy::template impl<T>(v));                                        \
    }                                                                                         \
    return result;                                                                            \
}

namespace methodverse::parameter {

    DEFINE_VECTOR_BINARY_FUNC(operator+, add_op)
    DEFINE_VECTOR_BINARY_FUNC(operator-, sub_op)
    DEFINE_VECTOR_BINARY_FUNC(operator*, mul_op)
    DEFINE_VECTOR_BINARY_FUNC(operator/, div_op)

    DEFINE_VECTOR_BINARY_FUNC(dot, dot_op)
    DEFINE_VECTOR_BINARY_FUNC(cross, cross_op)

    DEFINE_VECTOR_BINARY_FUNC(coefw_mul, coefw_mul_op)
    DEFINE_VECTOR_BINARY_FUNC(coefw_div, coefw_div_op)

    DEFINE_VECTOR_UNARY_FUNC(transpose, transpose_op)
    DEFINE_VECTOR_UNARY_FUNC(inverse, inverse_op)

}; // namespace methodverse::parameter

#undef DEFINE_BINARY_FUNC
#undef DEFINE_VECTOR_BINARY_FUNC