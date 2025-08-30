// tests/test_primitive_operators.cpp
#include <gtest/gtest.h>
#include <type_traits>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "primitive_operators.h"

using namespace mv;

// ---------- Detection utilities ----------
// Test Alias<T> can actually be specialized.
template<template<class> class Alias, class T, class = void>
struct is_instantiable0 : std::false_type {};

template<template<class> class Alias, class T>
struct is_instantiable0<Alias, T, std::void_t<Alias<T>>> : std::true_type {};

template<template<class> class Alias, class T>
concept is_instantiable = requires { typename Alias<T>;};// requires { typename Alias<T>; };

template<class Alias>
concept is_present = requires { typename Alias; };

// ---------- Compile-time formation tests ----------
TEST(PolicyFormation, ScalarPolicy_WellFormed)
{
    // Scalars should be able to do these
    boost::mp11::mp_for_each<scalar_tag::types>(
        []<class T>(T) {
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template plus_int, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template plus_double, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template plus_vec, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template plus_mat, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template minus_int, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template minus_double, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template minus_vec, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template minus_mat, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template multiply_int, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template multiply_double, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template multiply_vec, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template multiply_mat, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template divide_int, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template divide_double, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template divide_vec, T>);
            static_assert(is_instantiable<OperationPolicy<scalar_tag>::template divide_mat, T>);
        }
    );
}

TEST(PolicyFormation, BoolPolicy_WellFormed)
{
    // Bool should be able to do these
    boost::mp11::mp_for_each<bool_tag::types>(
        []<class T>(T) {
            static_assert(is_present<OperationPolicy<bool_tag>::and_bool>);
            static_assert(is_present<OperationPolicy<bool_tag>::or_bool>);
            static_assert(is_present<OperationPolicy<bool_tag>::xor_bool>);
            static_assert(is_present<OperationPolicy<bool_tag>::xnor_bool>);
            static_assert(is_present<OperationPolicy<bool_tag>::not_bool>);
        }
    );
}

TEST(PolicyFormation, StringPolicy_WellFormed)
{
    // String should be able to do these
    boost::mp11::mp_for_each<string_tag::types>(
        []<class T>(T) {
            static_assert(is_present<OperationPolicy<string_tag>::plus_string>);
        }
    );
}

TEST(PolicyFormation, VectorPolicy_WellFormed)
{
    // Vector should be able to do these
    using colvec_identities = boost::mp11::mp_transform<boost::mp11::mp_identity, eigen_colvec_tag::types>;
    boost::mp11::mp_for_each<colvec_identities>(
        []<class T>(boost::mp11::mp_identity<T>) {
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template plus_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template plus_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template plus_self_vec_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template minus_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template minus_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template plus_self_vec_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template multiply_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template multiply_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template multiply_trans_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template multiply_self_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template divide_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template divide_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template dot_self_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template corss_self_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_colvec_tag>::template transpose, T>);
        }
    ); 
    
    using rowvec_identities = boost::mp11::mp_transform<boost::mp11::mp_identity, eigen_rowvec_tag::types>;
    boost::mp11::mp_for_each<rowvec_identities>(
        []<class T>(boost::mp11::mp_identity<T>) {
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template plus_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template plus_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template plus_self_vec_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template minus_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template minus_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template plus_self_vec_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template multiply_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template multiply_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template multiply_trans_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template multiply_self_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template divide_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template divide_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template dot_self_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template corss_self_t, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template transpose, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_rowvec_tag>::template divide_mat, T>); // only row vector can do this
        }
    );
}

TEST(PolicyFormation, QuaternionPolicy_WellFormed)
{
    using rowvec_identities = boost::mp11::mp_transform<boost::mp11::mp_identity, eigen_quat_tag::types>;
    boost::mp11::mp_for_each<rowvec_identities>(
        []<class T>(boost::mp11::mp_identity<T>) {
            static_assert(is_instantiable<OperationPolicy<eigen_quat_tag>::template multiply_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_quat_tag>::template multiply_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_quat_tag>::template multiply_quat, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_quat_tag>::template divide_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_quat_tag>::template divide_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_quat_tag>::template divide_quat, T>);
        }
    );
}

TEST(PolicyFormation, MatrixPolicy_WellFormed)
{
    using matrix_identities = boost::mp11::mp_transform<boost::mp11::mp_identity, eigen_mat_tag::types>;
    // Matrix should be able to do these
    boost::mp11::mp_for_each<matrix_identities>(
        []<class T>(boost::mp11::mp_identity<T>) {
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template plus_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template plus_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template plus_mat, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template minus_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template minus_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template minus_mat, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template multiply_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template multiply_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template multiply_mat, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template multiply_vec, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template multiply_mat_coefw, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template divide_int, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template divide_double, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template divide_mat, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template transpose, T>);
            static_assert(is_instantiable<OperationPolicy<eigen_mat_tag>::template inverse, T>);
        }
    );
}
