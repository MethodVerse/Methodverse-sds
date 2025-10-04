// oeraation_policy.h
// This file defines operator policies for categories. following policies are defined:
// addition (+): scalar+scalar, scalar+eigen, string+string, etc
// subtraction (-): scalar-scalar, scalar-eigen, etc
// multiplication (*): scala*scalar, scalar*eigen, etc
// division (/): scalar/scalar, scalar/eigen, eigen/scalar, eigen/eigen, etc
// corss product (x): eigen x eigen (only for eigen_colvec_tag, eigen_rowvec_tag)
// dot product (.): eigen . eigen (only for eigen_colvec_tag, eigen_rowvec_tag)
// coefficient wise multiplication: only for eigen types
// transpose (.T): eigen only
// inverse (.inv()): eigen_mat_tag only
// boolean ops (&&, ||, !, xor, xnor): bool only
// Author: Chenguang Zhao
// Date: 2025-08-29

#pragma once

#include <string>
#include <utility> 
#include <cmath>
#include <type_traits>
#include <concepts> 
#include <iostream>
#include "tags.h"

namespace methodverse::parameter {

    ////////////////////////// addition operator + //////////////////////////
    // ---- scalar + scalar - > scalar
    template<>
    struct op_policy<scalar_tag, scalar_tag, add_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions        
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && is_category_of<T2, scalar_tag>)
        static std::common_type_t<T1,T2> impl(T1 const &s1, T2 const &s2) { 
            using C = std::common_type_t<T1,T2>;
            return static_cast<C>(s1) + static_cast<C>(s2); 
        }

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
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>>)
        static T2 impl(T1 const &s, T2 const &vm) { return (static_cast<double>(s) + vm.array()).eval();}
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
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && is_category_of<T2, scalar_tag>)
        static T1 impl(T1 const &vm, T2 const &s) { return (vm.array() + static_cast<double>(s)).eval(); }
        
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };
    template<> struct op_policy<eigen_colvec_tag, scalar_tag, add_op> : public op_policy<eigen_vecmat_tag, scalar_tag, add_op> {};
    template<> struct op_policy<eigen_rowvec_tag, scalar_tag, add_op> : public op_policy<eigen_vecmat_tag, scalar_tag, add_op> {};
    template<> struct op_policy<eigen_mat_tag, scalar_tag, add_op> : public op_policy<eigen_vecmat_tag, scalar_tag, add_op> {};

    // ---- eigen + eigen - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, eigen_vecmat_tag, add_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>> && std::is_same_v<T1, T2>)
        static T1 impl(T1 const &vm1, T2 const &vm2) { return (vm1.array() + vm2.array()).eval(); }
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };
    template<> struct op_policy<eigen_colvec_tag, eigen_colvec_tag, add_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, add_op> {};
    template<> struct op_policy<eigen_rowvec_tag, eigen_rowvec_tag, add_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, add_op> {};
    template<> struct op_policy<eigen_mat_tag, eigen_mat_tag, add_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, add_op> {};

    // ---- quaternion + quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, add_op> {
        static constexpr bool enabled = true;
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_quat_tag> && is_category_of<T2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(T1 const &q1, T2 const &q2) { return Eigen::Quaterniond(q1.coeffs() + q2.coeffs()); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };

    // ---- string + string - > string
    template<>
    struct op_policy<string_tag, string_tag, add_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_same_v<T1, std::string> && std::is_same_v<T2, std::string>)
        static std::string impl(T1 const &s1, T2 const &s2) { return s1 + s2; }

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
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && is_category_of<T2, scalar_tag>)
        static std::common_type_t<T1,T2> impl(T1 const &s1, T2 const &s2) { return static_cast<double>(s1) - static_cast<double>(s2); }

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
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>>)
        static T2 impl(T1 const &s, T2 const &vm) { return (static_cast<double>(s) - vm.array()).eval(); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };
    template<> struct op_policy<scalar_tag, eigen_colvec_tag, sub_op> : public op_policy<scalar_tag, eigen_vecmat_tag, sub_op> {};
    template<> struct op_policy<scalar_tag, eigen_rowvec_tag, sub_op> : public op_policy<scalar_tag, eigen_vecmat_tag, sub_op> {};
    template<> struct op_policy<scalar_tag, eigen_mat_tag, sub_op> : public op_policy<scalar_tag, eigen_vecmat_tag, sub_op> {};

    // ---- eigen - scalar - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, scalar_tag, sub_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && is_category_of<T2, scalar_tag>)
        static T1 impl(T1 const &vm, T2 const &s) { return ( vm.array() - static_cast<double>(s)).eval(); }
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };
    template<> struct op_policy<eigen_colvec_tag, scalar_tag, sub_op> : public op_policy<eigen_vecmat_tag, scalar_tag, sub_op> {};
    template<> struct op_policy<eigen_rowvec_tag, scalar_tag, sub_op> : public op_policy<eigen_vecmat_tag, scalar_tag, sub_op> {};
    template<> struct op_policy<eigen_mat_tag, scalar_tag, sub_op> : public op_policy<eigen_vecmat_tag, scalar_tag, sub_op> {};

    // ---- eigen - eigen - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, eigen_vecmat_tag, sub_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>> && std::is_same_v<T1, T2>)
        static T1 impl(T1 const &vm1, T2 const &vm2) { return (vm1.array() - vm2.array()).eval(); }
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        requires ( Ux == Uy ) // units must be the same
        static consteval auto unit_of() { return Ux; }
    };
    template<> struct op_policy<eigen_colvec_tag, eigen_colvec_tag, sub_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, sub_op> {};
    template<> struct op_policy<eigen_rowvec_tag, eigen_rowvec_tag, sub_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, sub_op> {};
    template<> struct op_policy<eigen_mat_tag, eigen_mat_tag, sub_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, sub_op> {};

    // ---- quaternion - quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, sub_op> {
        static constexpr bool enabled = true;
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_quat_tag> && is_category_of<T2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(T1 const &q1, T2 const &q2) { return Eigen::Quaterniond(q1.coeffs() - q2.coeffs()); }

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
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && is_category_of<T2, scalar_tag>)
        static std::common_type_t<T1,T2> impl(T1 const &s1, T2 const &s2) { return static_cast<double>(s1) * static_cast<double>(s2); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; }
    };

    // ---- scalar * eigen - > eigen
    template<>
    struct op_policy<scalar_tag, eigen_vecmat_tag, mul_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>>)
        static T2 impl(T1 const &s, T2 const &vm) { return (static_cast<double>(s) * vm.array()).eval(); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; }
    };
    template<> struct op_policy<scalar_tag, eigen_colvec_tag, mul_op> : public op_policy<scalar_tag, eigen_vecmat_tag, mul_op> {};
    template<> struct op_policy<scalar_tag, eigen_rowvec_tag, mul_op> : public op_policy<scalar_tag, eigen_vecmat_tag, mul_op> {};
    template<> struct op_policy<scalar_tag, eigen_mat_tag, mul_op> : public op_policy<scalar_tag, eigen_vecmat_tag, mul_op> {};
    
    // ---- eigen * scalar - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, scalar_tag, mul_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && is_category_of<T2, scalar_tag>)
        static T1 impl(T1 const &vm, T2 const &s) { return (vm.array() * static_cast<double>(s)).eval(); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };
    template<> struct op_policy<eigen_colvec_tag, scalar_tag, mul_op> : public op_policy<eigen_vecmat_tag, scalar_tag, mul_op> {};
    template<> struct op_policy<eigen_rowvec_tag, scalar_tag, mul_op> : public op_policy<eigen_vecmat_tag, scalar_tag, mul_op> {};
    template<> struct op_policy<eigen_mat_tag, scalar_tag, mul_op> : public op_policy<eigen_vecmat_tag, scalar_tag, mul_op> {};

    // ---- rowvec * colvec -> scalar
    template<>
    struct op_policy<eigen_rowvec_tag, eigen_colvec_tag, mul_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_rowvec_tag> && 
                  is_category_of<T2, eigen_colvec_tag> && 
                  (T1::ColsAtCompileTime == T2::RowsAtCompileTime))
        static double impl(T1 const &rv, T2 const &cv) { return (rv * cv).value(); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };

    // ---- colvec * rowvec -> matrix
    template<>
    struct op_policy<eigen_colvec_tag, eigen_rowvec_tag, mul_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_colvec_tag> && 
                  is_category_of<T2, eigen_rowvec_tag> && 
                  (T1::RowsAtCompileTime == T2::ColsAtCompileTime))
        static auto impl(T1 const &cv, T2 const &rv) { return (cv * rv).eval(); }   
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };

    // ---- matrix * matrix -> matrix
    template<>
    struct op_policy<eigen_mat_tag, eigen_mat_tag, mul_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_mat_tag> && is_category_of<T2, eigen_mat_tag> && (T1::ColsAtCompileTime == T2::RowsAtCompileTime))
        static auto impl(T1 const &m1, T2 const &m2) { return (m1 * m2).eval(); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };
    // ---- matrix * colvec -> colvec
    template<>
    struct op_policy<eigen_mat_tag, eigen_colvec_tag, mul_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_mat_tag> && is_category_of<T2, eigen_colvec_tag> && (T1::ColsAtCompileTime == T2::RowsAtCompileTime))
        static auto impl(T1 const &m, T2 const &cv) { return (m * cv).eval(); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; }
    };

    // ---- rowvec * matrix -> rowvec
    template<>
    struct op_policy<eigen_rowvec_tag, eigen_mat_tag, mul_op> {
        static constexpr bool enabled = true;   
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_rowvec_tag> && is_category_of<T2, eigen_mat_tag> && (T1::ColsAtCompileTime == T2::RowsAtCompileTime))
        static auto impl(T1 const &rv, T2 const &m) { return (rv * m).eval(); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; }
    };

    // ---- quaternion * quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, mul_op> {
        static constexpr bool enabled = true;
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_quat_tag> && is_category_of<T2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(T1 const &q1, T2 const &q2) { return q1 * q2; }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux*Uy; }
    };

    ////////////////////////// Dividion operator / //////////////////////////
    // ---- scalar / scalar - > scalar
    template<>
    struct op_policy<scalar_tag, scalar_tag, div_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && is_category_of<T2, scalar_tag>)
        static std::common_type_t<T1,T2> impl(T1 const &s1, T2 const &s2) { 
            using C = std::common_type_t<T1,T2>;
            return static_cast<C>(s1) / static_cast<C>(s2); 
        }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }
    };

    // ---- scalar / eigen - > eigen
    template<>
    struct op_policy<scalar_tag, eigen_vecmat_tag, div_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, scalar_tag> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>>)
        static T2 impl(T1 const &s, T2 const &vm) { return (static_cast<double>(s) / vm.array()).eval(); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }
    };
    template<> struct op_policy<scalar_tag, eigen_colvec_tag, div_op> : public op_policy<scalar_tag, eigen_vecmat_tag, div_op> {};
    template<> struct op_policy<scalar_tag, eigen_rowvec_tag, div_op> : public op_policy<scalar_tag, eigen_vecmat_tag, div_op> {};
    template<> struct op_policy<scalar_tag, eigen_mat_tag, div_op> : public op_policy<scalar_tag, eigen_vecmat_tag, div_op> {};

    // ---- eigen / scalar - > eigen
    template<>
    struct op_policy<eigen_vecmat_tag, scalar_tag, div_op> {
        static constexpr bool enabled = true;

        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && is_category_of<T2, scalar_tag>)
        static T1 impl(T1 const &vm, T2 const &s) { return ( vm.array() / static_cast<double>(s)).eval(); }

        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }

    };
    template<> struct op_policy<eigen_colvec_tag, scalar_tag, div_op> : public op_policy<eigen_vecmat_tag, scalar_tag, div_op> {};
    template<> struct op_policy<eigen_rowvec_tag, scalar_tag, div_op> : public op_policy<eigen_vecmat_tag, scalar_tag, div_op> {};
    template<> struct op_policy<eigen_mat_tag, scalar_tag, div_op> : public op_policy<eigen_vecmat_tag, scalar_tag, div_op> {};

    // ---- rowvec / matrix -> rowvec
    template<>
    struct op_policy<eigen_rowvec_tag, eigen_mat_tag, div_op> {
        static constexpr bool enabled = true;   
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_rowvec_tag> && is_category_of<T2, eigen_mat_tag> && (T1::ColsAtCompileTime == T2::RowsAtCompileTime))
        static auto impl(T1 const &rv, T2 const &m) { return (rv * m.inverse()).eval(); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }
    };

    // ---- matrix / matrix -> matrix
    template<>
    struct op_policy<eigen_mat_tag, eigen_mat_tag, div_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_mat_tag> && is_category_of<T2, eigen_mat_tag> && (T1::ColsAtCompileTime == T2::RowsAtCompileTime))
        static auto impl(T1 const &m1, T2 const &m2) { return (m1 * m2.inverse()).eval(); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; } // multiplication of units
    };

    // ---- quaternion / quaternion - > quaternion
    template<>
    struct op_policy<eigen_quat_tag, eigen_quat_tag, div_op> {
        static constexpr bool enabled = true;

        template <class T1, class T2>
        requires (is_category_of<T1, eigen_quat_tag> && is_category_of<T2, eigen_quat_tag>)
        static Eigen::Quaterniond impl(T1 const &q1, T2 const &q2) { return q1 * q2.inverse(); }

        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; }
    };

    ////////////////////////// coefficient wise multiplication //////////////////////////
    // ---- eigen .* eigen -> eigen
    template<>
    struct op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_mul_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>> && std::is_same_v<T1, T2>)
        static auto impl(T1 const &vm1, T2 const &vm2) { return (vm1.array() * vm2.array()).matrix().eval(); }
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };

    template<> struct op_policy<eigen_colvec_tag, eigen_colvec_tag, coefw_mul_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_mul_op> {};
    template<> struct op_policy<eigen_rowvec_tag, eigen_rowvec_tag, coefw_mul_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_mul_op> {};
    template<> struct op_policy<eigen_mat_tag, eigen_mat_tag, coefw_mul_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_mul_op> {};

    ////////////////////////// coefficient wise divide //////////////////////////
    // ---- eigen ./ eigen -> eigen
    template<>
    struct op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_div_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>> && std::is_base_of_v<eigen_vecmat_tag, category_t<T2>> && std::is_same_v<T1, T2>)
        static auto impl(T1 const &vm1, T2 const &vm2) { return (vm1.array() / vm2.array()).eval(); }
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux / Uy; } // multiplication of units
    };

    template<> struct op_policy<eigen_colvec_tag, eigen_colvec_tag, coefw_div_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_div_op> {};
    template<> struct op_policy<eigen_rowvec_tag, eigen_rowvec_tag, coefw_div_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_div_op> {};
    template<> struct op_policy<eigen_mat_tag, eigen_mat_tag, coefw_div_op> : public op_policy<eigen_vecmat_tag, eigen_vecmat_tag, coefw_div_op> {};
    
    ///////////////////////////// cross //////////////////////////
    // ---- eigen cross eigen -> eigen
    template<>
    struct op_policy<eigen_colvec_tag, eigen_colvec_tag, cross_op>
    {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_colvec_tag> && is_category_of<T2, eigen_colvec_tag> && (T1::RowsAtCompileTime == 3) && (T2::RowsAtCompileTime == 3))
        static auto impl(T1 const &v1, T2 const &v2) { return v1.cross(v2); }
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };
    
    template<>
    struct op_policy<eigen_rowvec_tag, eigen_rowvec_tag, cross_op>
    {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (is_category_of<T1, eigen_rowvec_tag> && is_category_of<T2, eigen_rowvec_tag> && (T1::ColsAtCompileTime == 3) && (T2::ColsAtCompileTime == 3))
        static auto impl(T1 const &v1, T2 const &v2) { return (v1.transpose().eval().cross(v2.transpose())).transpose().eval(); }
        // Units of two parameters must be the same for addition operation
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };

    ///////////////////////////// policies for bool only //////////////////////////
    // ---- bool && bool -> bool
    template<>
    struct op_policy<bool_tag, bool_tag, and_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>)
        static bool impl(T1 const &b1, T2 const &b2) { return b1 && b2; }
    };

    // ---- bool || bool -> bool
    template<>
    struct op_policy<bool_tag, bool_tag, or_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>)
        static bool impl(T1 const &b1, T2 const &b2) { return b1 || b2; }
    };

    // ---- bool xor bool -> bool
    template<>
    struct op_policy<bool_tag, bool_tag, xor_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>)
        static bool impl(T1 const &b1, T2 const &b2) { return b1 != b2; }
    };

    // ---- bool xnor bool -> bool
    template<>
    struct op_policy<bool_tag, bool_tag, xnor_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>)
        static bool impl(T1 const &b1, T2 const &b2) { return b1 == b2; }
    };

    // ---- not bool -> bool
    template<>
    struct op_policy<bool_tag, void, not_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1>
        requires (std::is_same_v<T1, bool>)
        static bool impl(T1 const &b) { return !b; }
    };

    ///////////////////////////// dot operation //////////////////////////
    // ---- dot of two vectors -> scalar
    template<>
    struct op_policy<eigen_vec_tag, eigen_vec_tag, dot_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1, class T2>
        requires std::is_base_of_v<eigen_vec_tag, category_t<T1>> && std::is_same_v<T1, T2>
        static double impl(T1 const &v1, T2 const &v2) { return v1.dot(v2); }
        template <auto Ux, auto Uy>
        static consteval auto unit_of() { return Ux * Uy; } // multiplication of units
    };
    template<> struct op_policy<eigen_colvec_tag, eigen_colvec_tag, dot_op> : public op_policy<eigen_vec_tag, eigen_vec_tag, dot_op> {};
    template<> struct op_policy<eigen_rowvec_tag, eigen_rowvec_tag, dot_op> : public op_policy<eigen_vec_tag, eigen_vec_tag, dot_op> {};

    ///////////////////////////// transpose //////////////////////////
    // ---- transpose of matrix -> matrix
    template<>
    struct op_policy<eigen_vecmat_tag, void, transpose_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1>
        requires (std::is_base_of_v<eigen_vecmat_tag, category_t<T1>>)
        static auto impl(T1 const &m) { return m.transpose().eval(); }
        template <auto Ux>
        static consteval auto unit_of() { return Ux; } // unit remains the same
    };

    template<> struct op_policy<eigen_colvec_tag, void, transpose_op> : public op_policy<eigen_vecmat_tag, void, transpose_op> {};
    template<> struct op_policy<eigen_rowvec_tag, void, transpose_op> : public op_policy<eigen_vecmat_tag, void, transpose_op> {};
    template<> struct op_policy<eigen_mat_tag, void, transpose_op> : public op_policy<eigen_vecmat_tag, void, transpose_op> {};

    ///////////////////////////// inverse //////////////////////////
    // ---- inverse of matrix -> matrix
    template<>  
    struct op_policy<eigen_mat_tag, void, inverse_op> {
        static constexpr bool enabled = true;
        // Implementation body as templated free/static functions
        template <class T1>
        requires (is_category_of<T1, eigen_mat_tag>)
        static auto impl(T1 const &m) { return m.inverse().eval(); }
        template <auto Ux>
        static consteval auto unit_of() { return 1 / Ux; } // inverse of unit
    };

    // ---- return type deduction helper
    template <class Policy, class T1, class T2 = void, class = void>
    struct op_return_type { using type = void; };

    // return type traits for binary operation
    template <class Policy, class T1, class T2>
    struct op_return_type<Policy, T1, T2,
                          std::void_t<decltype(Policy::template impl<T1, T2>(
                              std::declval<T1>(), std::declval<T2>()))>> {
        using type = decltype(Policy::template impl<T1, T2>(std::declval<T1>(), std::declval<T2>()));
    };

    // return type traits for unary operation
    template <class Policy, class T1>
    struct op_return_type<Policy, T1, void,
                          std::void_t<decltype(Policy::template impl<T1>(
                              std::declval<T1>()))>> {
        using type = decltype(Policy::template impl<T1>(std::declval<T1>()));
    };

    template <class Policy, class T1, class T2 = void>
    using op_return_t = typename op_return_type<Policy, T1, T2>::type;

    template <class Policy, class T1, class T2 = void, class UR = op_return_t<Policy, T1, T2>>
    concept op_allowed = Policy::enabled && !std::is_void_v<op_return_t<Policy, T1, T2>>;

    // This is used to prevent the compiler from applying generic operators (+ - * /) to std::vector<T>
    template<class T> struct is_std_vector : std::false_type {}; 
    template<class T, class A> struct is_std_vector<std::vector<T,A>> : std::true_type {}; 
    template<class T> inline constexpr bool is_std_vector_v = is_std_vector<T>::value;
}; // namespace methodverse::parameter
