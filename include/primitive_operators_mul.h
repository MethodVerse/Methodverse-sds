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
    // ---- scalar * scalar - > scalar
    template<>
    struct op_policy<scalar_tag, scalar_tag, mul_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && is_category_of<U2, scalar_tag>)
        static double impl(U1 const &s1, U2 const &s2) { return static_cast<double>(s1) * static_cast<double>(s2); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };

    // ---- scalar * eigen - > eigen
    template<>
    struct op_policy<scalar_tag, eigen_vecmat_tag, mul_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class U1, class U2>
        requires (is_category_of<U1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, typename category<U2>::type>)
        static U2 impl(U1 const &s, U2 const &vm) { return (static_cast<double>(s) * vm.array()).eval(); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
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
}; // namespace mv
