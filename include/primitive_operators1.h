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

    // ---- op tags & category tags
    struct mul_op {};
    struct add_op {};
    struct sub_op {};
    struct div_op {};
    struct neg_op {};
    struct not_op {};
    struct and_op {};
    struct or_op {};
    struct xor_op {};
    struct xnor_op {};
    struct transpose_op {};
    struct inverse_op {};
    struct coefw_mul_op {};
    struct dot_op {};
    struct cross_op {};

    template<class T> inline constexpr bool always_false = false;

    // ---- Define category tags for different primitive types. purpose is to organize some types into one category
    struct scalar_tag { using types = boost::mp11::mp_list<int, double>;}; // scalar types include everthing that is convertable to double
    struct string_tag { using types = boost::mp11::mp_list<std::string>;};
    struct bool_tag { using types = boost::mp11::mp_list<bool>;};
    struct eigen_vecmat_tag {};
    struct eigen_vec_tag : public eigen_vecmat_tag{};
    struct eigen_quat_tag { using types = boost::mp11::mp_list<Eigen::Quaterniond>;};
    struct eigen_colvec_tag : public eigen_vec_tag { using types = boost::mp11::mp_list<Eigen::Vector3d>;};    // all column vectors
    struct eigen_rowvec_tag : public eigen_vec_tag { using types = boost::mp11::mp_list<Eigen::RowVector3d>;}; // all row vectors
    struct eigen_mat_tag { using types = boost::mp11::mp_list<Eigen::Matrix3d>;};       // all matrices

    template<class T, class Tag> 
    concept is_category_of = boost::mp11::mp_contains<typename Tag::types, T>::value;

template<class T>
struct category {
    using type =
        std::conditional_t<is_category_of<T, scalar_tag>,      scalar_tag,
        std::conditional_t<is_category_of<T, string_tag>,      string_tag,
        std::conditional_t<is_category_of<T, bool_tag>,        bool_tag,
        std::conditional_t<is_category_of<T, eigen_quat_tag>,  eigen_quat_tag,
        std::conditional_t<is_category_of<T, eigen_colvec_tag>,eigen_colvec_tag,
        std::conditional_t<is_category_of<T, eigen_rowvec_tag>,eigen_rowvec_tag,
        std::conditional_t<is_category_of<T, eigen_mat_tag>,   eigen_mat_tag,
        void>>>>>>>;
    static_assert(!std::is_same_v<type, void>, "Type not in any category");
};

    using primitive_types = boost::mp11::mp_list<
        bool, 
        std::string, 
        int, 
        double,
        Eigen::Vector3d, 
        Eigen::RowVector3d, 
        Eigen::Matrix3d, 
        Eigen::Quaterniond>;

    template <class T>
    concept is_allowed_primitive = boost::mp11::mp_contains<primitive_types, T>::value;

    // ---- traits: variant index of a type
    template <class V, class T, std::size_t I = 0>
    consteval std::size_t variant_index_of() {
        if constexpr (I == std::variant_size_v<V>)
            return std::variant_npos;
        else if constexpr (std::is_same_v<T, std::variant_alternative_t<I, V>>)
            return I;
        else
            return variant_index_of<V, T, I + 1>();
    }

    // ---- result type of a function pointer. we need this is that the operations on two types may return 
    template<class> struct fnptr_result;
    template<class R, class... A> struct fnptr_result<R(*)(A...)> { using type = R; };

    template<class C1, class C2, class Op>
    struct op_policy { static constexpr bool enabled = false; };
}; // namespace mv
