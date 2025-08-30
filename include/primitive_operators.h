// primitive_operators.h
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

namespace mv {

    // Define category tags for different primitive types. purpose is to organize some types into one category
    struct scalar_tag { using types = boost::mp11::mp_list<int, double>;};
    struct string_tag { using types = boost::mp11::mp_list<std::string>;};
    struct bool_tag { using types = boost::mp11::mp_list<bool>;};
    struct eigen_quat_tag { using types = boost::mp11::mp_list<Eigen::Quaterniond>;};
    struct eigen_colvec_tag { using types = boost::mp11::mp_list<Eigen::Vector3d>;};
    struct eigen_rowvec_tag { using types = boost::mp11::mp_list<Eigen::RowVector3d>;};
    struct eigen_mat_tag { using types = boost::mp11::mp_list<Eigen::Matrix3d>;};

    template<class T> concept is_allowed_scalar = boost::mp11::mp_contains<scalar_tag::types, T>::value;
    template<class T> concept is_allowed_string = boost::mp11::mp_contains<string_tag::types, T>::value;
    template<class T> concept is_allowed_bool = boost::mp11::mp_contains<bool_tag::types, T>::value;
    template<class T> concept is_allowed_quat = boost::mp11::mp_contains<eigen_quat_tag::types, T>::value;
    template<class T> concept is_allowed_colvec = boost::mp11::mp_contains<eigen_colvec_tag::types, T>::value;
    template<class T> concept is_allowed_rowvec = boost::mp11::mp_contains<eigen_rowvec_tag::types, T>::value;
    template<class T> concept is_allowed_mat = boost::mp11::mp_contains<eigen_mat_tag::types, T>::value;

    template <class T, class = void>
    struct category {
        using type = scalar_tag;
    };
    template <class T>
    struct category<T, std::enable_if_t<std::is_convertible_v<std::remove_cvref_t<T>, std::string>>> {
         using type = string_tag;
    };
    template <>
    struct category<bool> {
        using type = bool_tag;
    };
    template <class T>
    struct category<T, std::void_t< // Check if T is an Eigen type
                           decltype(std::remove_cvref_t<T>::RowsAtCompileTime),
                           decltype(std::remove_cvref_t<T>::ColsAtCompileTime)>> {
        using U = std::remove_cvref_t<T>;
        using type = std::conditional_t<
            U::RowsAtCompileTime == 1, eigen_rowvec_tag,
            std::conditional_t<U::ColsAtCompileTime == 1, eigen_colvec_tag, eigen_mat_tag>>;
    };
    template<class T>
    struct category<T, std::enable_if_t<std::is_same_v<std::remove_cvref_t<T>, Eigen::Quaterniond>>>{
        using type = eigen_quat_tag;
    };
    template <class T>
    using category_t = typename category<std::remove_cvref_t<T>>::type;
    using primitive_types = boost::mp11::mp_list<
        bool, std::string, int, double,
        Eigen::Vector3d, Eigen::RowVector3d, Eigen::Matrix3d, Eigen::Quaterniond>;
    template <class T>
    concept is_allowed_primitive = boost::mp11::mp_contains<primitive_types, T>::value;

    // Primary binary operator template: P1 and P2 are the primitive types, R is return type, op is the operator functor
    // Op(const P1&, const P2&) -> R
    template <class P1, class P2, class R, class Op> 
    requires (is_allowed_primitive<P1> && is_allowed_primitive<P2> && is_allowed_primitive<R>) &&
    requires (P1 a, P2 b, Op op) { { op(a, b) } -> std::convertible_to<R>;}
    struct binary_op {
        R operator()(const P1 &p1, const P2 &p2) const { return Op{}(p1, p2); }
    };

    // Primary unary operatory tmplate, P is the primitive type, R is return type, op is the operator functor
    // Op(const P&) -> R
    template<class P, class R, class Op>
    requires (is_allowed_primitive<P> && is_allowed_primitive<R>) &&
    requires (P a, Op op) { { op(a) } -> std::convertible_to<R>;}
    struct unary_op {
        R operator()(const P &p) const { return Op{}(p); }
    };

    // Add eigen and scalar operation support
    struct eigen_plus_scalar  {
        template <class S>
        auto operator()(const Eigen::Vector3d &v, const S &s) const { return (v.array() + static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const Eigen::Matrix3d &m, const S &s) const { return (m.array() + static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const Eigen::RowVector3d &m, const S &s) const { return (m.array() + static_cast<double>(s)).matrix(); }        
        template <class S>
        auto operator()(const S &s, const Eigen::Vector3d &v) const { return (v.array() + static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::Matrix3d &m) const { return (m.array() + static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::RowVector3d &m) const { return (m.array() + static_cast<double>(s)).matrix(); }        
    };

    struct eigen_minus_scalar  {
        template <class S>
        auto operator()(const Eigen::Vector3d &v, const S &s) const { return (v.array() - static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const Eigen::Matrix3d &m, const S &s) const { return (m.array() - static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const Eigen::RowVector3d &m, const S &s) const { return (m.array() - static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::Vector3d &v) const { return (static_cast<double>(s) - v.array()).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::Matrix3d &m) const { return (static_cast<double>(s) - m.array() ).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::RowVector3d &m) const { return (static_cast<double>(s) - m.array() ).matrix(); }
    };

    struct eigen_divide_scalar {
        template <class S>
        auto operator()(const Eigen::Vector3d &v, const S &s) const { return (v.array() / static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const Eigen::Matrix3d &m, const S &s) const { return (m.array() / static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const Eigen::RowVector3d &m, const S &s) const { return (m.array() / static_cast<double>(s)).matrix(); }
        template <class S>
        auto operator()(const Eigen::Quaterniond& q, const S &s) const { 
            const double k = static_cast<double>(s);
            return Eigen::Quaterniond(q.w()/k, q.x()/k, q.y()/k, q.z()/k); // c
        }
        template <class S>
        auto operator()(const S &s, const Eigen::Vector3d &v) const { return (static_cast<double>(s) / v.array()).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::Matrix3d &m) const { return (static_cast<double>(s) / m.array()).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::RowVector3d &m) const { return (static_cast<double>(s) / m.array()).matrix(); }
        template <class S>
        auto operator()(const S &s, const Eigen::Quaterniond& q) const { 
            const double k = static_cast<double>(s);
            return Eigen::Quaterniond(k/q.w(), k/q.x(), k/q.y(), k/q.z()); // c 
        }
    };

    struct eigen_multiply_scalar {
        template <class S>
        auto operator()(const Eigen::Quaterniond &q, const S &s) const { 
            auto k = static_cast<double>(s);
            return Eigen::Quaterniond(q.w()*k, q.x()*k, q.y()*k, q.z()*k); // c
        }
        template <class S>
        auto operator()(const S &s, const Eigen::Quaterniond &q) const { 
            auto k = static_cast<double>(s);
            return Eigen::Quaterniond(q.w()*k, q.x()*k, q.y()*k, q.z()*k); // c
        }
    };
    
    struct eigen_transpose {
        Eigen::RowVector3d operator()(const Eigen::Vector3d &v) const { return v.transpose().eval(); }
        Eigen::Vector3d operator()(const Eigen::RowVector3d &v) const { return v.transpose().eval(); }
        Eigen::Matrix3d operator()(const Eigen::Matrix3d &m) const { return m.transpose().eval(); }
    };

    struct eigen_dot_vec {
        double operator()(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2) const { return v1.dot(v2); }
        double operator()(const Eigen::RowVector3d& v1, const Eigen::RowVector3d& v2) const { return v1.dot(v2); }
    };

    struct eigen_cross_vec {
        Eigen::Vector3d operator()(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2) const { return v1.cross(v2); }
        Eigen::RowVector3d operator()(const Eigen::RowVector3d& v1, const Eigen::RowVector3d& v2) const { return (v1.transpose().eval().cross(v2.transpose())).transpose().eval(); }
    };

    struct eigen_multiply_eigen {
        Eigen::Vector3d operator()(const Eigen::Matrix3d& m, const Eigen::Vector3d& v) const { return (m * v).eval(); }
        Eigen::RowVector3d operator()(const Eigen::RowVector3d& v, const Eigen::Matrix3d& m) const { return (v * m).eval(); }
        Eigen::Matrix3d operator()(const Eigen::Vector3d& vv, const Eigen::RowVector3d& vh) const { return (vv * vh).eval(); }
        double operator()(const Eigen::RowVector3d& vh, const Eigen::Vector3d& vv) const { return vh * vv; }
    };

    struct eigen_divide_eigen {
        Eigen::Matrix3d operator()(const Eigen::Matrix3d &m1, const Eigen::Matrix3d &m2) const { 
            return m2.transpose().colPivHouseholderQr().solve(m1.transpose()).transpose().eval(); }
        Eigen::RowVector3d operator()(const Eigen::RowVector3d &v, const Eigen::Matrix3d &m) const { 
            return m.transpose().colPivHouseholderQr().solve(v.transpose()).transpose().eval(); }
        Eigen::Quaterniond operator()(const Eigen::Quaterniond  &q1, const Eigen::Quaterniond  &q2) const {
            return q1 * q2.inverse(); // q1 / q2
        }
    };

    struct eigen_coefwise_eigen {
        Eigen::Matrix3d operator()(const Eigen::Matrix3d &m1, const Eigen::Matrix3d &m2) const {
            return (m1.array() * m2.array()).matrix().eval();
        }
        Eigen::Vector3d operator()(const Eigen::Vector3d &v1, const Eigen::Vector3d &v2) const {
            return (v1.array() * v2.array()).matrix().eval();
        }
        Eigen::RowVector3d operator()(const Eigen::RowVector3d &v1, const Eigen::RowVector3d &v2) const {
            return (v1.array() * v2.array()).matrix().eval();
        }
    };

    struct eigen_inverse {
        Eigen::Matrix3d operator()(const Eigen::Matrix3d &m) const {
            return m.inverse().eval();
        }
    };

    template <class Tag>
    struct OperationPolicy;

    template<>
    struct OperationPolicy<scalar_tag> {        
        // plus
        template<class T> requires is_allowed_scalar<T>
        using plus_int = binary_op<T, int, T, std::plus<>>;
        template<class T> requires is_allowed_scalar<T>
        using plus_double = binary_op<T, double, T, std::plus<>>;
        template<class T> requires is_allowed_scalar<T>
        using plus_vec = binary_op<T, Eigen::Vector3d, Eigen::Vector3d, eigen_plus_scalar>;
        template<class T> requires is_allowed_scalar<T>
        using plus_mat = binary_op<T, Eigen::Matrix3d, Eigen::Matrix3d, eigen_plus_scalar>;   

        // minus
        template<class T> requires is_allowed_scalar<T>
        using minus_int = binary_op<T, int, T, std::minus<>>;
        template<class T> requires is_allowed_scalar<T>
        using minus_double = binary_op<T, double, T, std::minus<>>;
        template<class T> requires is_allowed_scalar<T>
        using minus_vec = binary_op<T, Eigen::Vector3d, Eigen::Vector3d, eigen_minus_scalar>;
        template<class T> requires is_allowed_scalar<T>
        using minus_mat = binary_op<T, Eigen::Matrix3d, Eigen::Matrix3d, eigen_minus_scalar>;    
        
        // multiply
        template<class T> requires is_allowed_scalar<T>
        using multiply_int = binary_op<T, int, T, std::multiplies<>>;
        template<class T> requires is_allowed_scalar<T>
        using multiply_double = binary_op<T, double, T, std::multiplies<>>;
        template<class T> requires is_allowed_scalar<T>
        using multiply_vec = binary_op<T, Eigen::Vector3d, Eigen::Vector3d, eigen_plus_scalar>;
        template<class T> requires is_allowed_scalar<T>
        using multiply_mat = binary_op<T, Eigen::Matrix3d, Eigen::Matrix3d, eigen_plus_scalar>;   

        // divide
        template<class T> requires is_allowed_scalar<T>
        using divide_int = binary_op<T, int, T, std::divides<>>;
        template<class T> requires is_allowed_scalar<T>
        using divide_double = binary_op<T, double, T, std::divides<>>;
        template<class T> requires is_allowed_scalar<T>
        using divide_vec = binary_op<T, Eigen::Vector3d, Eigen::Vector3d, eigen_divide_scalar>;
        template<class T> requires is_allowed_scalar<T>
        using divide_mat = binary_op<T, Eigen::Matrix3d, Eigen::Matrix3d, eigen_divide_scalar>;                 
    };

    // Add boolean xor and xnor operation
    struct logical_xor {        
        template <class T, class U>
        constexpr auto operator()(T &&lhs, U &&rhs) const
            noexcept(noexcept(static_cast<bool>(std::forward<T>(lhs)) !=
                              static_cast<bool>(std::forward<U>(rhs)))) -> bool {
            return static_cast<bool>(std::forward<T>(lhs)) !=
                   static_cast<bool>(std::forward<U>(rhs));
        }
    };
    struct logical_xnor {
        template <class T, class U>
        constexpr auto operator()(T &&lhs, U &&rhs) const
            noexcept(noexcept(static_cast<bool>(std::forward<T>(lhs)) ==
                              static_cast<bool>(std::forward<U>(rhs)))) -> bool {
            return static_cast<bool>(std::forward<T>(lhs)) ==
                   static_cast<bool>(std::forward<U>(rhs));
        }
    };

    template<>
    struct OperationPolicy<string_tag> {        
        using plus_string = binary_op<std::string, std::string, std::string, std::plus<>>;
    };

    template<>
    struct OperationPolicy<bool_tag> {        
        using and_bool = binary_op<bool, bool, bool, std::logical_and<>>;
        using or_bool = binary_op<bool, bool, bool, std::logical_or<>>;
        using xor_bool = binary_op<bool, bool, bool, logical_xor>;
        using xnor_bool = binary_op<bool, bool, bool, logical_xnor>;
        using not_bool = unary_op<bool, bool, std::logical_not<>>;
    }; 

    // 1) traits describing each vector "shape": vertical or horizontal.
    template <class Tag> struct eigen_vec_traits;

    template <>
    struct eigen_vec_traits<eigen_colvec_tag> {
        using vec_t = Eigen::Vector3d;          // own shape
        using xvec_t = Eigen::RowVector3d;      // transpose shape
        using mtimes_t = Eigen::Matrix3d;       // vec * xvec -> matrix
    };

    template <>
    struct eigen_vec_traits<eigen_rowvec_tag>
    {
        using vec_t = Eigen::RowVector3d;       // own shape
        using xvec_t = Eigen::Vector3d;         // transpose shape
        using mtimes_t = double;                // row * col -> 1x1 -> double
    };

    // 2) shared base: all the duplicated aliases for vectors live here
    template <class Tag>
    struct EigenVectorPolicyBase {
        using traits = eigen_vec_traits<Tag>;
        using vec_t = typename traits::vec_t;
        using xvec_t = typename traits::xvec_t;
        using mtimes_t = typename traits::mtimes_t;
    
        // plus
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using plus_int = binary_op<T, int, T, eigen_plus_scalar>;
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using plus_double = binary_op<T, double, T, eigen_plus_scalar>;
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using plus_self_vec_t = binary_op<T, T, T, std::plus<>>;

        // minus
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using minus_int = binary_op<T, int, T, eigen_minus_scalar>;
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using minus_double = binary_op<T, double, T, eigen_minus_scalar>;
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using minus_self_vec_t = binary_op<T, T, T, std::minus<>>;

        // multiply
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using multiply_int = binary_op<T, int, T, std::multiplies<>>;
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using multiply_double = binary_op<T, double, T, std::multiplies<>>;
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using multiply_trans_t = binary_op<T, xvec_t, mtimes_t, eigen_multiply_eigen>; // multiply with transpose, either expend to matrix or reduce to scalar.
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using multiply_self_t = binary_op<T, T, T, eigen_coefwise_eigen>;              // multiply with self type, coefficient-wise.

        // divide
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using divide_int = binary_op<T, int, T, std::divides<>>;
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using divide_double = binary_op<T, double, T, std::divides<>>;

        // dot
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using dot_self_t = binary_op<T, T, double, eigen_dot_vec>;

        // cross
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using corss_self_t = binary_op<T, T, T, eigen_cross_vec>;

        // transpose
        template <class T> requires (is_allowed_colvec<T> || is_allowed_rowvec<T>)
        using transpose = unary_op<T, xvec_t, eigen_transpose>;
    };

    template<> struct OperationPolicy<eigen_colvec_tag> : EigenVectorPolicyBase<eigen_colvec_tag> {};
    template<> struct OperationPolicy<eigen_rowvec_tag> : EigenVectorPolicyBase<eigen_rowvec_tag> {
        // divide matrix
        template <class T> requires is_allowed_rowvec<T>
        using divide_mat = binary_op<T, Eigen::Matrix3d, T, eigen_divide_eigen>;
    };

    template<> struct OperationPolicy<eigen_quat_tag> {
        // plus
        // minus
        // multiply
        template <class T> requires is_allowed_quat<T>
        using multiply_int = binary_op<T, int, T, eigen_multiply_scalar>;
        template <class T> requires is_allowed_quat<T>
        using multiply_double = binary_op<T, double, T, eigen_multiply_scalar>;
        template <class T> requires is_allowed_quat<T>
        using multiply_quat = binary_op<T, Eigen::Quaterniond, Eigen::Quaterniond, std::multiplies<>>;

        // divide
        template <class T> requires is_allowed_quat<T>
        using divide_int = binary_op<T, int, T, eigen_divide_scalar>;
        template <class T> requires is_allowed_quat<T>
        using divide_double = binary_op<T, double, T, eigen_divide_scalar>;
        template <class T> requires is_allowed_quat<T>
        using divide_quat = binary_op<T, Eigen::Quaterniond, Eigen::Quaterniond, eigen_divide_eigen>;
    };

    template <>
    struct OperationPolicy<eigen_mat_tag> {
        // plus
        template <class T> requires is_allowed_mat<T>
        using plus_int = binary_op<T, int, T, eigen_plus_scalar>;
        template <class T> requires is_allowed_mat<T>
        using plus_double = binary_op<T, double, T, eigen_plus_scalar>;
        template <class T> requires is_allowed_mat<T>
        using plus_mat = binary_op<T, Eigen::Matrix3d, Eigen::Matrix3d, std::plus<>>;

        // minus
        template <class T> requires is_allowed_mat<T>
        using minus_int = binary_op<T, int, T, eigen_minus_scalar>;
        template <class T> requires is_allowed_mat<T>
        using minus_double = binary_op<T, double, T, eigen_minus_scalar>;
        template <class T> requires is_allowed_mat<T>
        using minus_mat = binary_op<T, T, T, std::minus<>>;

        // multiply
        template <class T> requires is_allowed_mat<T>
        using multiply_int = binary_op<T, int, T, std::multiplies<>>;
        template <class T> requires is_allowed_mat<T>
        using multiply_double = binary_op<T, double, T, std::multiplies<>>;
        template <class T> requires is_allowed_mat<T>
        using multiply_vec = binary_op<T, Eigen::Vector3d, Eigen::Vector3d, std::multiplies<>>;
        template <class T> requires is_allowed_mat<T>
        using multiply_mat = binary_op<T, Eigen::Matrix3d, Eigen::Matrix3d, std::multiplies<>>;
        template <class T> requires is_allowed_mat<T>
        using multiply_mat_coefw = binary_op<T, Eigen::Matrix3d, Eigen::Matrix3d, eigen_coefwise_eigen>;

        // divide
        template <class T> requires is_allowed_mat<T>
        using divide_int = binary_op<T, int, T, std::divides<>>;
        template <class T> requires is_allowed_mat<T>
        using divide_double = binary_op<T, double, T, std::divides<>>;
        template <class T> requires is_allowed_mat<T>
        using divide_mat = binary_op<T, T, T, eigen_divide_eigen>;

        // transpose
        template <class T> requires is_allowed_mat<T>
        using transpose = unary_op<T, T, eigen_transpose>;

        // inverse
        template <class T> requires is_allowed_mat<T>
        using inverse = unary_op<T, T, eigen_inverse>;
    };
}; // namespace mv
