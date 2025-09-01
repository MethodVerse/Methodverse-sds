// primitive_operators_add.h
// This file defines operators for primitive types, such as int, bool, double, Eigen::Vector3d, etc.
// Author: Chenguang Zhao
// Date: 2025-08-29

#pragma once
#include <string>
#include <utility> 
#include <Eigen/Dense>
#include <cmath>
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <type_traits>
#include <concepts> 
#include "primitive_operators1.h"

namespace mv {
    ////////////////////////// addition operator + //////////////////////////
    // ---- scalar + scalar - > scalar
    template<>
    struct op_policy<scalar_tag, scalar_tag, add_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && is_category_of<U2, scalar_tag>)
        static std::common_type_t<U1,U2> impl(U1 const &s1, U2 const &s2) { return static_cast<double>(s1) + static_cast<double>(s2); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };

    // ---- scalar + eigen - > eigen
    template<>
    struct op_policy<scalar_tag, eigen_vecmat_tag, add_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, typename category<U2>::type>)
        static U2 impl(U1 const &s, U2 const &vm) { return (static_cast<double>(s) + vm.array()).eval(); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };
    template<> struct op_policy<scalar_tag, eigen_colvec_tag, add_op> : public op_policy<scalar_tag, eigen_vecmat_tag, add_op> {};
    template<> struct op_policy<scalar_tag, eigen_rowvec_tag, add_op> : public op_policy<scalar_tag, eigen_vecmat_tag, add_op> {};
    template<> struct op_policy<scalar_tag, eigen_mat_tag, add_op> : public op_policy<scalar_tag, eigen_vecmat_tag, add_op> {};

    // ---- eigen + scalar - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, scalar_tag, add_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (std::is_base_of_v<eigen_vecmat_tag, typename category<U1>::type> && is_category_of<U2, scalar_tag>)
        static U1 impl(U1 const &vm, U2 const &s) { return (vm.array() + static_cast<double>(s)).eval(); }
        
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };
    template<> struct op_policy<eigen_colvec_tag, scalar_tag, add_op> : public op_policy<eigen_vecmat_tag, scalar_tag, add_op> {};
    template<> struct op_policy<eigen_rowvec_tag, scalar_tag, add_op> : public op_policy<eigen_vecmat_tag, scalar_tag, add_op> {};
    template<> struct op_policy<eigen_mat_tag, scalar_tag, add_op> : public op_policy<eigen_vecmat_tag, scalar_tag, add_op> {};

    // ---- quaternion + quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, add_op> {
        static constexpr bool enabled = true;
        template <class U1, class U2>
        requires (is_category_of<U1, eigen_quat_tag> && is_category_of<U2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(U1 const &q1, U2 const &q2) { return Eigen::Quaterniond(q1.coeffs() + q2.coeffs()); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };

    ////////////////////////// subtraction operator - //////////////////////////
    // ---- scalar - scalar - > scalar
    template<>
    struct op_policy<scalar_tag, scalar_tag, sub_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && is_category_of<U2, scalar_tag>)
        static std::common_type_t<U1,U2> impl(U1 const &s1, U2 const &s2) { return static_cast<double>(s1) - static_cast<double>(s2); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };

    // ---- scalar - eigen - > eigen
    template<>
    struct op_policy<scalar_tag, eigen_vecmat_tag, sub_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, typename category<U2>::type>)
        static U2 impl(U1 const &s, U2 const &vm) { return (static_cast<double>(s) - vm.array()).eval(); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };

    // ---- eigen - scalar - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, scalar_tag, sub_op> : public op_policy<scalar_tag, eigen_vecmat_tag, sub_op> {};

    // ---- quaternion - quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, sub_op> {
        static constexpr bool enabled = true;
        template <class U1, class U2>
        requires (is_category_of<U1, eigen_quat_tag> && is_category_of<U2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(U1 const &q1, U2 const &q2) { return Eigen::Quaterniond(q1.coeffs() - q2.coeffs()); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };    


    ////////////////////////// multiplication operator * //////////////////////////
    // ---- scalar * scalar - > scalar
    template<>
    struct op_policy<scalar_tag, scalar_tag, mul_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && is_category_of<U2, scalar_tag>)
        static std::common_type_t<U1,U2> impl(U1 const &s1, U2 const &s2) { return static_cast<double>(s1) * static_cast<double>(s2); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; }
    };

    // ---- scalar * eigen - > eigen
    template<>
    struct op_policy<scalar_tag, eigen_vecmat_tag, mul_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, typename category<U2>::type>)
        static U2 impl(U1 const &s, U2 const &vm) { return (static_cast<double>(s) * vm.array()).eval(); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; }
    };

    // ---- eigen * scalar - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, scalar_tag, mul_op> : public op_policy<scalar_tag, eigen_vecmat_tag, mul_op> {};

    // ---- quaternion * quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, mul_op> {
        static constexpr bool enabled = true;
        template <class U1, class U2>
        requires (is_category_of<U1, eigen_quat_tag> && is_category_of<U2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(U1 const &q1, U2 const &q2) { return q1 * q2; }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };

    ////////////////////////// Dividion operator / //////////////////////////
    // ---- scalar / scalar - > scalar
    template<>
    struct op_policy<scalar_tag, scalar_tag, div_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && is_category_of<U2, scalar_tag>)
        static double impl(U1 const &s1, U2 const &s2) { return static_cast<double>(s1) / static_cast<double>(s2); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }
    };

    // ---- scalar / eigen - > eigen
    template<>
    struct op_policy<scalar_tag, eigen_vecmat_tag, div_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, typename category<U2>::type>)
        static U2 impl(U1 const &s, U2 const &vm) { return (static_cast<double>(s) / vm.array()).eval(); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }
    };

    // ---- eigen / scalar - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, scalar_tag, div_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (std::is_base_of_v<eigen_vecmat_tag, typename category<U1>::type> && is_category_of<U2, scalar_tag>)
        static U1 impl(U1 const &vm, U2 const &s) { return ( vm.array() / static_cast<double>(s)).eval(); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }

    };
    template<> struct op_policy<eigen_colvec_tag, scalar_tag, div_op> : public op_policy<eigen_vecmat_tag, scalar_tag, div_op> {};
    template<> struct op_policy<eigen_rowvec_tag, scalar_tag, div_op> : public op_policy<eigen_vecmat_tag, scalar_tag, div_op> {};
    template<> struct op_policy<eigen_mat_tag, scalar_tag, div_op> : public op_policy<eigen_vecmat_tag, scalar_tag, div_op> {};

    // ---- quaternion / quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, div_op> {
        static constexpr bool enabled = true;

        template <class U1, class U2>
        requires (is_category_of<U1, eigen_quat_tag> && is_category_of<U2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(U1 const &q1, U2 const &q2) { return q1 * q2.inverse(); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) 
        static consteval auto unit_of() { return Ux / Uy; }
    };

    // ---- return type deduction helper
    template <class Policy, class U1, class U2, class = void>
    struct op_return_type { static_assert(always_false<Policy>, "Policy does not define impl<U1,U2>");};

    template <class Policy, class U1, class U2>
    struct op_return_type<Policy, U1, U2,
                          std::void_t<decltype(Policy::template impl<U1, U2>(
                              std::declval<U1>(), std::declval<U2>()))>> {
        using type = decltype(Policy::template impl<U1, U2>(std::declval<U1>(), std::declval<U2>()));
    };

    template <class Policy, class U1, class U2>
    using op_return_t = typename op_return_type<Policy, U1, U2>::type;

    template <class Policy, class U1, class U2, class UR = op_return_t<Policy, U1, U2>>
    concept op_allowed = Policy::enabled && std::is_same_v<UR, op_return_t<Policy, U1, U2>>;

    // ---- parameter precheck macro
    // This macro is to be used inside operator overload functions to do static checks on parameters and return type
    // OP_TAG: the operation tag, e.g. add_op
    // OP_NAME: the operation name in string, e.g. "operator+" for error messages
    #define PARAMETER_BINARY_PRECHECK(OP_TAG, OP_NAME)                                    \
        using category_lhs = typename category<T>::type;                                  \
        using category_rhs = typename category<typename D2::value_type>::type;            \
        using value_type_lhs = T;                                                         \
        using value_type_rhs = typename D2::value_type;                                   \
        using value_type_ret = typename DR::value_type;                                   \
                                                                                          \
        using policy = op_policy<category_lhs, category_rhs, OP_TAG>;                     \
        using return_type_policy = op_return_t<policy, value_type_lhs, value_type_rhs>;   \
                                                                                          \
        constexpr auto unit_lhs = Base::GetUnit();                                        \
        constexpr auto unit_rhs = D2::GetUnit();                                          \
        constexpr auto unit_policy = policy::template unit_of<unit_lhs, unit_rhs>();      \
        constexpr auto unit_ret = DR::GetUnit();                                          \
                                                                                          \
        static_assert(std::is_same_v<return_type_policy, value_type_ret>,                 \
                    "Return type mismatch in " OP_NAME);                                  \
        static_assert(policy::enabled, "Operation not enabled in " OP_NAME);              \
        static_assert(op_allowed<policy, value_type_lhs, value_type_rhs, value_type_ret>, \
                    "Operation not allowed in " OP_NAME);                                 \
        static_assert(unit_policy == unit_ret, "Unit mismatch in " OP_NAME)
}; // namespace mv
