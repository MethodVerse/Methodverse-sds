// parameterbase_operator.h
// This file defines operators on Parameter like classs.
// Author: Chenguang Zhao
// Date: 2025-08-29

#pragma once

#include "operator.h"
#include "tags.h"
#include "parameterbase.h"

// In namespace mv (or fully qualify mv:: in the macro if used elsewhere)
#define DEFINE_PARAMETER_BINARY_FUNC(FUNC_NAME, OP_TAG)                                       \
template <mv::ParameterLike L, mv::ParameterLike R>                                           \
requires requires (const L& lhs, const R& rhs) {                                              \
  /* require that the policy impl for these value types is well-formed */                     \
  mv::op_policy<                                                                              \
      mv::category_t<typename L::value_type>,                                                 \
      mv::category_t<typename R::value_type>,                                                 \
      OP_TAG                                                                                  \
  >::template impl<typename L::value_type, typename R::value_type>(lhs.Val(), rhs.Val());     \
}                                                                                             \
auto FUNC_NAME (const L& lhs, const R& rhs) {                                                 \
  using TL = typename L::value_type;                                                          \
  using TR = typename R::value_type;                                                          \
  using Policy = mv::op_policy<mv::category_t<TL>, mv::category_t<TR>, OP_TAG>;               \
  static_assert(Policy::enabled, #FUNC_NAME " not enabled for these types");                  \
                                                                                              \
  using RetT = mv::op_return_t<Policy, TL, TR>;                                               \
  constexpr auto Ux = L::GetUnit();                                                           \
  constexpr auto Uy = R::GetUnit();                                                           \
  constexpr auto RetU = Policy::template unit_of<Ux, Uy>();                                   \
                                                                                              \
  auto v = FUNC_NAME(lhs.Get(), rhs.Get());                                                   \
  return mv::ParameterBase<RetT, RetU>(v);                                                    \
}

// In namespace mv (or fully qualify mv:: in the macro if used elsewhere)
#define DEFINE_PARAMETER_UNARY_FUNC(FUNC_NAME, OP_TAG)                                        \
template <mv::ParameterLike P>                                                                \
requires requires (const p& p) {                                                              \
  /* require that the policy impl for these value types is well-formed */                     \
  mv::op_policy<                                                                              \
      mv::category_t<typename P::value_type>,                                                 \
      void,                                                                                   \
      OP_TAG                                                                                  \
  >::template impl<typename P::value_type>(p.Val());                                          \
}                                                                                             \
auto FUNC_NAME(const P& p) {                                                                  \
  using T = typename P::value_type;                                                           \
  using Policy = mv::op_policy<mv::category_t<T>, void, OP_TAG>;                              \
  static_assert(Policy::enabled, #FUNC_NAME " not enabled for these types");                  \
                                                                                              \
  using RetT = mv::op_return_t<Policy, T>;                                                    \
  constexpr auto Ux = L::GetUnit();                                                           \
  constexpr auto RetU = Policy::template unit_of<Ux>();                                       \
                                                                                              \
  auto v = FUNC_NAME(p.Get());                                                                \
  return mv::ParameterBase<RetT, RetU>(v);                                                    \
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