// parameterbase_operator.h
// This file defines operators on Parameter like classs.
// Author: Chenguang Zhao
// Date: 2025-08-29

#pragma once

#include "operator.h"
#include "tags.h"
#include "parameterbase.h"

#define DEFINE_PARAMETER_BINARY_FUNC(FUNC_NAME, OP_TAG)                                       \
template <class T1, class T2, mp_units::Reference auto U1, mp_units::Reference auto U2>   \
auto FUNC_NAME (const ParameterBase<T1, U1>& lhs, const ParameterBase<T2, U2>& rhs) \
requires requires (const T1& lhs, const T2& rhs) {                                              \
  /* require that the policy impl for these value types is well-formed */                     \
  op_policy<                                                                              \
      category_t<T1>,                                                 \
      category_t<T2>,                                                 \
      OP_TAG                                                                                  \
  >::template impl<T1, T2>(lhs, rhs);     \
}                                                                                             \
{                                                 \
  using Policy = op_policy<category_t<T1>, category_t<T2>, OP_TAG>;               \
  static_assert(Policy::enabled, #FUNC_NAME " not enabled for these types");                  \
                                                                                              \
  using RetT = op_return_t<Policy, T1, T2>;                                               \
  constexpr auto RetU = Policy::template unit_of<U1, U2>();                                   \
                                                                                              \
  auto v = array_##OP_TAG(lhs.Get(), rhs.Get());                                                   \
  return ParameterBase<RetT, RetU>(v);                                                    \
}

// In namespace mv (or fully qualify mv:: in the macro if used elsewhere)
#define DEFINE_PARAMETER_UNARY_FUNC(FUNC_NAME, OP_TAG)                                        \
template <ParameterLike P>                                                                \
requires requires (const P& p) {                                                              \
  /* require that the policy impl for these value types is well-formed */                     \
  op_policy<                                                                              \
      category_t<typename P::value_type>,                                                 \
      void,                                                                                   \
      OP_TAG                                                                                  \
  >::template impl<typename P::value_type>(p.Val());                                          \
}                                                                                             \
auto FUNC_NAME(const P& p) {                                                                  \
  using T = typename P::value_type;                                                           \
  using Policy = op_policy<category_t<T>, void, OP_TAG>;                              \
  static_assert(Policy::enabled, #FUNC_NAME " not enabled for these types");                  \
                                                                                              \
  using RetT = op_return_t<Policy, T>;                                                    \
  constexpr auto Ux = P::GetUnit();                                                           \
  constexpr auto RetU = Policy::template unit_of<Ux>();                                       \
                                                                                              \
  auto v = array_##OP_TAG(p.Get());                                                                \
  return ParameterBase<RetT, RetU>(v);                                                    \
}

namespace methodverse::parameter {

  template <class P>
  concept ParameterLike = requires(const P &x) {
    typename P::value_type;
    { P::GetUnit() }; // static constexpr/consteval
    { x.Val() };      // value getter
  };

  DEFINE_PARAMETER_BINARY_FUNC(operator+, add_op)
  DEFINE_PARAMETER_BINARY_FUNC(operator-, sub_op)
  DEFINE_PARAMETER_BINARY_FUNC(operator*, mul_op)
  DEFINE_PARAMETER_BINARY_FUNC(operator/, div_op)

  DEFINE_PARAMETER_BINARY_FUNC(dot, dot_op)
  DEFINE_PARAMETER_BINARY_FUNC(cross, cross_op)

  DEFINE_PARAMETER_BINARY_FUNC(coefw_mul, coefw_mul_op)
  DEFINE_PARAMETER_BINARY_FUNC(coefw_div, coefw_div_op)

  DEFINE_PARAMETER_UNARY_FUNC(transpose, transpose_op)
  DEFINE_PARAMETER_UNARY_FUNC(inverse, inverse_op)
};