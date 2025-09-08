// tst/operator_test.h
// Defines types used for primitive operator test
// Author: Chenguang Zhao
// Date: 2025-09-06

#pragma once

#include <gtest/gtest.h>
#include <boost/mp11.hpp>
#include <type_traits>
#include <vector>
#include <string>
#include <Eigen/Dense>

#include <methodverse/parameter/operator.h>   // your operators/macros live here

using namespace methodverse::parameter;

namespace methodverse::operatortest
{
    // ---------- Pair wrapper & cartesian product ----------
    template <class L, class R, class RES>
    struct Sample
    {
        using left_type = L;
        using right_type = R;
        using result_type = RES;
        left_type left;
        right_type right;
        result_type result;
        Sample(const L &l, const R &r, const RES &res) : left(l), right(r), result(res) {}
    };
    template <class L, class R>
    struct Sample<L, R, void>
    {
        using left_type = L;
        using right_type = R;
        using result_type = void;
        left_type left;
        right_type right;
    };

    using DBL = double;
    using INT = int;
    using BOO = bool;
    using STR = std::string;
    using CV3 = Eigen::Vector3d;
    using RV3 = Eigen::RowVector3d;
    using MAT3 = Eigen::Matrix3d;
    using QUAD = Eigen::Quaterniond;

    template <class L, class R>
    struct Pair
    {
        using left_type = L;
        using right_type = R;
    };

    using AllTypes = boost::mp11::mp_list<DBL, INT, BOO, STR, CV3, RV3, MAT3, QUAD>;
    using AllPairs = boost::mp11::mp_product<Pair, AllTypes, AllTypes>;

    using AllPairsT = boost::mp11::mp_apply<::testing::Types, AllPairs>;

    // Detects types that support isApprox(other, eps) -> bool
    template<class T>
    concept HasIsApprox = requires(const T& a, const T& b, double eps) {
        { a.isApprox(b, eps) } -> std::convertible_to<bool>;
    };

    template <class A, class B>
    void ExpectEqualSmart(const A &expected, const B &actual, double eps = 1e-10) {
        if constexpr (HasIsApprox<A> && HasIsApprox<B>) {
            // Eigen matrices/arrays/quaternions, etc.
            EXPECT_TRUE(expected.isApprox(actual, eps))
                << "expected = " << expected
                << "\nactual   = " << actual
                << "\neps      = " << eps;
        }
        else if constexpr (std::is_floating_point_v<std::decay_t<A>> &&
                        std::is_floating_point_v<std::decay_t<B>>) {
            EXPECT_NEAR(expected, actual, eps);
        }
        else {
            // ints, bools, std::string, etc.
            EXPECT_EQ(expected, actual);
        }
    }
};