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

    namespace add{
        // ---- Create value pairs (N^2) with expected results, otherwise exception ----
        // ---- If operation is allowed, a meaningful expected return value pre-calculated is also returned as reference.
        // ---- Otherwise, void return type and runtime exception is expected.
        template <class T1, class T2, class T3> Sample<T1, T2, T3> makeSample();
        template<> inline Sample<INT, INT, INT> makeSample<INT, INT, INT>() { return Sample(2, 3, 5); }
        template<> inline Sample<INT, DBL, DBL> makeSample<INT, DBL, DBL>() { return Sample(2, 3.5, 5.5); }
        template<> inline Sample<INT, BOO, void> makeSample<INT, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, STR, void> makeSample<INT, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, RV3, RV3> makeSample<INT, RV3, RV3>() { return Sample(2, RV3(1.1,2.2,3.3), RV3(3.1,4.2,5.3)); }
        template<> inline Sample<INT, CV3, CV3> makeSample<INT, CV3, CV3>() { return Sample(2, CV3(1.1,2.2,3.3), CV3(3.1,4.2,5.3)); }
        template<> inline Sample<INT, MAT3, MAT3> makeSample<INT, MAT3, MAT3>() { return Sample(2, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 3,4,5,6,7,8,9,10,11).finished()); }
        template<> inline Sample<INT, QUAD, void> makeSample<INT, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<DBL, DBL, DBL> makeSample<DBL, DBL, DBL>() { return Sample(2.5, 3.5, 6.0); }
        template<> inline Sample<DBL, INT, DBL> makeSample<DBL, INT, DBL>() { return Sample(2.5, 3, 5.5); }
        template<> inline Sample<DBL, BOO, void> makeSample<DBL, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, STR, void> makeSample<DBL, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, RV3, RV3> makeSample<DBL, RV3, RV3>() { return Sample(2.5, RV3(1.1,2.2,3.3), RV3(3.6,4.7,5.8)); }
        template<> inline Sample<DBL, CV3, CV3> makeSample<DBL, CV3, CV3>() { return Sample(2.5, CV3(1.1,2.2,3.3), CV3(3.6,4.7,5.8)); }
        template<> inline Sample<DBL, MAT3, MAT3> makeSample<DBL, MAT3, MAT3>() { return Sample(2.5, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5).finished()); }
        template<> inline Sample<DBL, QUAD, void> makeSample<DBL, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<BOO, BOO, void> makeSample<BOO, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, INT, void> makeSample<BOO, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, DBL, void> makeSample<BOO, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, STR, void> makeSample<BOO, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, RV3, void> makeSample<BOO, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, CV3, void> makeSample<BOO, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, MAT3, void> makeSample<BOO, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, QUAD, void> makeSample<BOO, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<STR, STR, STR> makeSample<STR, STR, STR>() { return Sample(std::string("hello"), std::string("methodverse"), std::string("hellomethodverse")); }
        template<> inline Sample<STR, INT, void> makeSample<STR, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, DBL, void> makeSample<STR, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, BOO, void> makeSample<STR, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, RV3, void> makeSample<STR, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, CV3, void> makeSample<STR, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, MAT3, void> makeSample<STR, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, QUAD, void> makeSample<STR, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<RV3, RV3, RV3> makeSample<RV3, RV3, RV3>() { return Sample(RV3(1,2,3), RV3(4,5,6), RV3(5,7,9)); }
        template<> inline Sample<RV3, CV3, void> makeSample<RV3, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, INT, RV3> makeSample<RV3, INT, RV3>() { return Sample(RV3(1,2,3), 2, RV3(3,4,5)); }
        template<> inline Sample<RV3, DBL, RV3> makeSample<RV3, DBL, RV3>() { return Sample(RV3(1,2,3), 2.5, RV3(3.5,4.5,5.5)); }
        template<> inline Sample<RV3, BOO, void> makeSample<RV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, STR, void> makeSample<RV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, MAT3, void> makeSample<RV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, QUAD, void> makeSample<RV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<CV3, CV3, CV3> makeSample<CV3, CV3, CV3>() { return Sample(CV3(1,2,3), CV3(4,5,6), CV3(5,7,9)); }
        template<> inline Sample<CV3, RV3, void> makeSample<CV3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, INT, CV3> makeSample<CV3, INT, CV3>() { return Sample(CV3(1,2,3), 2, CV3(3,4,5)); }
        template<> inline Sample<CV3, DBL, CV3> makeSample<CV3, DBL, CV3>() { return Sample(CV3(1,2,3), 2.5, CV3(3.5,4.5,5.5)); }
        template<> inline Sample<CV3, BOO, void> makeSample<CV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, STR, void> makeSample<CV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, MAT3, void> makeSample<CV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, QUAD, void> makeSample<CV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<MAT3, MAT3, MAT3> makeSample<MAT3, MAT3, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 9,8,7,6,5,4,3,2,1).finished(), (MAT3() << 10,10,10,10,10,10,10,10,10).finished()); }
        template<> inline Sample<MAT3, INT, MAT3> makeSample<MAT3, INT, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2, (MAT3() << 3,4,5,6,7,8,9,10,11).finished()); }
        template<> inline Sample<MAT3, DBL, MAT3> makeSample<MAT3, DBL, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2.5, (MAT3() << 3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5).finished()); }
        template<> inline Sample<MAT3, BOO, void> makeSample<MAT3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, STR, void> makeSample<MAT3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, RV3, void> makeSample<MAT3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, CV3, void> makeSample<MAT3, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, QUAD, void> makeSample<MAT3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<QUAD, QUAD, QUAD> makeSample<QUAD, QUAD, QUAD>() { return Sample( QUAD(1, 2, 3, 4),  QUAD(4, 3, 2, 1),  QUAD(5, 5, 5, 5));}
        template<> inline Sample<QUAD, INT, void> makeSample<QUAD, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, DBL, void> makeSample<QUAD, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, BOO, void> makeSample<QUAD, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, STR, void> makeSample<QUAD, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, RV3, void> makeSample<QUAD, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, CV3, void> makeSample<QUAD, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, MAT3, void> makeSample<QUAD, MAT3, void>() { throw std::runtime_error("not supported!"); }
    };

    namespace sub {
        // ---- Create value pairs (N^2) with expected results, otherwise exception ----
        // ---- If operation is allowed, a meaningful expected return value pre-calculated is also returned as reference.
        // ---- Otherwise, void return type and runtime exception is expected.
        template <class T1, class T2, class T3> Sample<T1, T2, T3> makeSample();
        template<> inline Sample<INT, INT, INT> makeSample<INT, INT, INT>() { return Sample(2, 3, -1); }
        template<> inline Sample<INT, DBL, DBL> makeSample<INT, DBL, DBL>() { return Sample(2, 3.5, -1.5); }
        template<> inline Sample<INT, BOO, void> makeSample<INT, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, STR, void> makeSample<INT, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, RV3, RV3> makeSample<INT, RV3, RV3>() { return Sample(2, RV3(1.1,2.2,3.3), RV3(0.9,-0.2,-1.3)); }
        template<> inline Sample<INT, CV3, CV3> makeSample<INT, CV3, CV3>() { return Sample(2, CV3(1.1,2.2,3.3), CV3(0.9,-0.2,-1.3)); }
        template<> inline Sample<INT, MAT3, MAT3> makeSample<INT, MAT3, MAT3>() { return Sample(2, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 1,0,-1,-2,-3,-4,-5,-6,-7).finished()); }
        template<> inline Sample<INT, QUAD, void> makeSample<INT, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<DBL, DBL, DBL> makeSample<DBL, DBL, DBL>() { return Sample(2.5, 3.5, -1.0); }
        template<> inline Sample<DBL, INT, DBL> makeSample<DBL, INT, DBL>() { return Sample(2.5, 3, -0.5); }
        template<> inline Sample<DBL, BOO, void> makeSample<DBL, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, STR, void> makeSample<DBL, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, RV3, RV3> makeSample<DBL, RV3, RV3>() { return Sample(2.5, RV3(1.1,2.2,3.3), RV3(1.4,0.3,-0.8)); }
        template<> inline Sample<DBL, CV3, CV3> makeSample<DBL, CV3, CV3>() { return Sample(2.5, CV3(1.1,2.2,3.3), CV3(1.4,0.3,-0.8)); }
        template<> inline Sample<DBL, MAT3, MAT3> makeSample<DBL, MAT3, MAT3>() { return Sample(2.5, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 1.5,0.5,-0.5,-1.5,-2.5,-3.5,-4.5,-5.5,-6.5).finished()); }
        template<> inline Sample<DBL, QUAD, void> makeSample<DBL, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<BOO, BOO, void> makeSample<BOO, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, INT, void> makeSample<BOO, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, DBL, void> makeSample<BOO, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, STR, void> makeSample<BOO, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, RV3, void> makeSample<BOO, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, CV3, void> makeSample<BOO, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, MAT3, void> makeSample<BOO, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, QUAD, void> makeSample<BOO, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<STR, STR, void> makeSample<STR, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, INT, void> makeSample<STR, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, DBL, void> makeSample<STR, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, BOO, void> makeSample<STR, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, RV3, void> makeSample<STR, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, CV3, void> makeSample<STR, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, MAT3, void> makeSample<STR, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, QUAD, void> makeSample<STR, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<RV3, RV3, RV3> makeSample<RV3, RV3, RV3>() { return Sample(RV3(1,2,3), RV3(4,5,6), RV3(-3,-3,-3)); }
        template<> inline Sample<RV3, CV3, void> makeSample<RV3, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, INT, RV3> makeSample<RV3, INT, RV3>() { return Sample(RV3(1,2,3), 2, RV3(-1,0,1)); }
        template<> inline Sample<RV3, DBL, RV3> makeSample<RV3, DBL, RV3>() { return Sample(RV3(1,2,3), 2.5, RV3(-1.5,-0.5,0.5)); }
        template<> inline Sample<RV3, BOO, void> makeSample<RV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, STR, void> makeSample<RV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, MAT3, void> makeSample<RV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, QUAD, void> makeSample<RV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<CV3, CV3, CV3> makeSample<CV3, CV3, CV3>() { return Sample(CV3(1,2,3), CV3(4,5,6), CV3(-3,-3,-3)); }
        template<> inline Sample<CV3, RV3, void> makeSample<CV3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, INT, CV3> makeSample<CV3, INT, CV3>() { return Sample(CV3(1,2,3), 2, CV3(-1,0,1)); }
        template<> inline Sample<CV3, DBL, CV3> makeSample<CV3, DBL, CV3>() { return Sample(CV3(1,2,3), 2.5, CV3(-1.5,-0.5,0.5)); }
        template<> inline Sample<CV3, BOO, void> makeSample<CV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, STR, void> makeSample<CV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, MAT3, void> makeSample<CV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, QUAD, void> makeSample<CV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<MAT3, MAT3, MAT3> makeSample<MAT3, MAT3, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 9,8,7,6,5,4,3,2,1).finished(), (MAT3() << -8,-6,-4,-2,0,2,4,6,8).finished()); }
        template<> inline Sample<MAT3, INT, MAT3> makeSample<MAT3, INT, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2, (MAT3() << -1,0,1,2,3,4,5,6,7).finished()); }
        template<> inline Sample<MAT3, DBL, MAT3> makeSample<MAT3, DBL, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2.5, (MAT3() << -1.5,-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5).finished()); }
        template<> inline Sample<MAT3, BOO, void> makeSample<MAT3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, STR, void> makeSample<MAT3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, RV3, void> makeSample<MAT3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, CV3, void> makeSample<MAT3, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, QUAD, void> makeSample<MAT3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<QUAD, QUAD, QUAD> makeSample<QUAD, QUAD, QUAD>() { return Sample( QUAD(1, 2, 3, 4),  QUAD(4, 3, 2, 1),  QUAD(-3, -1, 1, 3));}
        template<> inline Sample<QUAD, INT, void> makeSample<QUAD, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, DBL, void> makeSample<QUAD, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, BOO, void> makeSample<QUAD, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, STR, void> makeSample<QUAD, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, RV3, void> makeSample<QUAD, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, CV3, void> makeSample<QUAD, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, MAT3, void> makeSample<QUAD, MAT3, void>() { throw std::runtime_error("not supported!"); }
    };

    namespace mul {
        // ---- Create value pairs (N^2) with expected results, otherwise exception ----
        // ---- If operation is allowed, a meaningful expected return value pre-calculated is also returned as reference.
        // ---- Otherwise, void return type and runtime exception is expected.
        template <class T1, class T2, class T3> Sample<T1, T2, T3> makeSample();
        template<> inline Sample<INT, INT, INT> makeSample<INT, INT, INT>() { return Sample(2, 3, 6); }
        template<> inline Sample<INT, DBL, DBL> makeSample<INT, DBL, DBL>() { return Sample(2, 3.5, 7.0); }
        template<> inline Sample<INT, BOO, void> makeSample<INT, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, STR, void> makeSample<INT, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, RV3, RV3> makeSample<INT, RV3, RV3>() { return Sample(2, RV3(1.1,2.2,3.3), RV3(2.2,4.4,6.6)); }
        template<> inline Sample<INT, CV3, CV3> makeSample<INT, CV3, CV3>() { return Sample(2, CV3(1.1,2.2,3.3), CV3(2.2,4.4,6.6)); }
        template<> inline Sample<INT, MAT3, MAT3> makeSample<INT, MAT3, MAT3>() { return Sample(2, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 2,4,6,8,10,12,14,16,18).finished()); }
        template<> inline Sample<INT, QUAD, void> makeSample<INT, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<DBL, DBL, DBL> makeSample<DBL, DBL, DBL>() { return Sample(2.5, 3.5, 8.75); }
        template<> inline Sample<DBL, INT, DBL> makeSample<DBL, INT, DBL>() { return Sample(2.5, 3, 7.5); }
        template<> inline Sample<DBL, BOO, void> makeSample<DBL, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, STR, void> makeSample<DBL, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, RV3, RV3> makeSample<DBL, RV3, RV3>() { return Sample(2.5, RV3(1.1,2.2,3.3), RV3(2.75,5.5,8.25)); }
        template<> inline Sample<DBL, CV3, CV3> makeSample<DBL, CV3, CV3>() { return Sample(2.5, CV3(1.1,2.2,3.3), CV3(2.75,5.5,8.25)); }
        template<> inline Sample<DBL, MAT3, MAT3> makeSample<DBL, MAT3, MAT3>() { return Sample(2.5, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() <<  2.5,5.,7.5,10.,12.5,15.,17.5,20.,22.5).finished()); }
        template<> inline Sample<DBL, QUAD, void> makeSample<DBL, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<BOO, BOO, void> makeSample<BOO, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, INT, void> makeSample<BOO, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, DBL, void> makeSample<BOO, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, STR, void> makeSample<BOO, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, RV3, void> makeSample<BOO, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, CV3, void> makeSample<BOO, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, MAT3, void> makeSample<BOO, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, QUAD, void> makeSample<BOO, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<STR, STR, void> makeSample<STR, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, INT, void> makeSample<STR, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, DBL, void> makeSample<STR, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, BOO, void> makeSample<STR, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, RV3, void> makeSample<STR, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, CV3, void> makeSample<STR, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, MAT3, void> makeSample<STR, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, QUAD, void> makeSample<STR, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<RV3, RV3, void> makeSample<RV3, RV3, void>() {  throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, CV3, DBL> makeSample<RV3, CV3, DBL>() {return Sample(RV3(1.1,-2.2,3.3), CV3(-2.75,5.5,-8.25), -42.35); }
        template<> inline Sample<RV3, INT, RV3> makeSample<RV3, INT, RV3>() { return Sample(RV3(1,2,3), 2, RV3(2,4,6)); }
        template<> inline Sample<RV3, DBL, RV3> makeSample<RV3, DBL, RV3>() { return Sample(RV3(1,2,3), 2.5, RV3(2.5,5,7.5)); }
        template<> inline Sample<RV3, BOO, void> makeSample<RV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, STR, void> makeSample<RV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, MAT3, RV3> makeSample<RV3, MAT3, RV3>() { return Sample(RV3(1.1,-2.2,3.3),(MAT3() << 1,2,3,4,5,6,7,8,9).finished(), RV3(15.4, 17.6, 19.8)); }
        template<> inline Sample<RV3, QUAD, void> makeSample<RV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<CV3, CV3, void> makeSample<CV3, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, RV3, MAT3> makeSample<CV3, RV3, MAT3>() { return Sample(CV3(1.1,-2.2,3.3), RV3(4,5,6), (MAT3() << 4.4,5.5,6.6,-8.8,-11.,-13.2,13.2,16.5,19.8).finished() ); }
        template<> inline Sample<CV3, INT, CV3> makeSample<CV3, INT, CV3>() { return Sample(CV3(1,2,3), 2, CV3(2,4,6)); }
        template<> inline Sample<CV3, DBL, CV3> makeSample<CV3, DBL, CV3>() { return Sample(CV3(1,2,3), 2.5, CV3(2.5,5,7.5)); }
        template<> inline Sample<CV3, BOO, void> makeSample<CV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, STR, void> makeSample<CV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, MAT3, void> makeSample<CV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, QUAD, void> makeSample<CV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<MAT3, MAT3, MAT3> makeSample<MAT3, MAT3, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 9,8,7,6,5,4,3,2,1).finished(), (MAT3() << 30,24,18,84,69,54,138,114,90).finished()); }
        template<> inline Sample<MAT3, INT, MAT3> makeSample<MAT3, INT, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2, (MAT3() << 2,4,6,8,10,12,14,16,18).finished()); }
        template<> inline Sample<MAT3, DBL, MAT3> makeSample<MAT3, DBL, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2.5, (MAT3() << 2.5,5.,7.5,10.,12.5,15.,17.5,20.,22.5).finished()); }
        template<> inline Sample<MAT3, BOO, void> makeSample<MAT3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, STR, void> makeSample<MAT3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, RV3, void> makeSample<MAT3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, CV3, CV3> makeSample<MAT3, CV3, CV3>() {  return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), CV3(1,-1,2), CV3(5,11,17)); }
        template<> inline Sample<MAT3, QUAD, void> makeSample<MAT3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<QUAD, QUAD, QUAD> makeSample<QUAD, QUAD, QUAD>() { return Sample( QUAD(1, 2, 3, 4),  QUAD(4, 3, 2, 1),  QUAD(-12, 6, 24, 12));}
        template<> inline Sample<QUAD, INT, void> makeSample<QUAD, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, DBL, void> makeSample<QUAD, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, BOO, void> makeSample<QUAD, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, STR, void> makeSample<QUAD, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, RV3, void> makeSample<QUAD, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, CV3, void> makeSample<QUAD, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, MAT3, void> makeSample<QUAD, MAT3, void>() { throw std::runtime_error("not supported!"); }
    };

    namespace div {
        // ---- Create value pairs (N^2) with expected results, otherwise exception ----
        // ---- If operation is allowed, a meaningful expected return value pre-calculated is also returned as reference.
        // ---- Otherwise, void return type and runtime exception is expected.
        template <class T1, class T2, class T3> Sample<T1, T2, T3> makeSample();
        template<> inline Sample<INT, INT, INT> makeSample<INT, INT, INT>() { return Sample(2, 3, 0); }
        template<> inline Sample<INT, DBL, DBL> makeSample<INT, DBL, DBL>() { return Sample(2, 3.5, 0.5714285714285714); }
        template<> inline Sample<INT, BOO, void> makeSample<INT, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, STR, void> makeSample<INT, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<INT, RV3, RV3> makeSample<INT, RV3, RV3>() { return Sample(2, RV3(2,4,8), RV3(1,0.5,0.25)); }
        template<> inline Sample<INT, CV3, CV3> makeSample<INT, CV3, CV3>() { return Sample(2, CV3(2,4,8), CV3(1,0.5,0.25)); }
        template<> inline Sample<INT, MAT3, MAT3> makeSample<INT, MAT3, MAT3>() { return Sample(2, (MAT3() << 1,2,4,8,16,-8,-4,-2,-1).finished(), (MAT3() << 2.,1.,0.5,0.25,0.125,-0.25,-0.5,-1.0,-2.).finished()); }
        template<> inline Sample<INT, QUAD, void> makeSample<INT, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<DBL, DBL, DBL> makeSample<DBL, DBL, DBL>() { return Sample(2.5, 5.0, 0.5); }
        template<> inline Sample<DBL, INT, DBL> makeSample<DBL, INT, DBL>() { return Sample(7.5, 3, 2.5); }
        template<> inline Sample<DBL, BOO, void> makeSample<DBL, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, STR, void> makeSample<DBL, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<DBL, RV3, RV3> makeSample<DBL, RV3, RV3>() { return Sample(2.5, RV3(2.5,5.0,10.0), RV3(1,0.5,0.25)); }
        template<> inline Sample<DBL, CV3, CV3> makeSample<DBL, CV3, CV3>() { return Sample(2.5, CV3(2.5,5.0,10.0), CV3(1,0.5,0.25)); }
        template<> inline Sample<DBL, MAT3, MAT3> makeSample<DBL, MAT3, MAT3>() { return Sample(2.5, (MAT3() << 1,2,4,8,16,-8,-4,-2,-1).finished(), (MAT3() << 2.5,1.25,0.625,0.3125,0.15625,-0.3125,-0.625,-1.25,-2.5).finished()); }
        template<> inline Sample<DBL, QUAD, void> makeSample<DBL, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<BOO, BOO, void> makeSample<BOO, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, INT, void> makeSample<BOO, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, DBL, void> makeSample<BOO, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, STR, void> makeSample<BOO, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, RV3, void> makeSample<BOO, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, CV3, void> makeSample<BOO, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, MAT3, void> makeSample<BOO, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<BOO, QUAD, void> makeSample<BOO, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<STR, STR, void> makeSample<STR, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, INT, void> makeSample<STR, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, DBL, void> makeSample<STR, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, BOO, void> makeSample<STR, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, RV3, void> makeSample<STR, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, CV3, void> makeSample<STR, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, MAT3, void> makeSample<STR, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<STR, QUAD, void> makeSample<STR, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<RV3, RV3, void> makeSample<RV3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, CV3, void> makeSample<RV3, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, INT, RV3> makeSample<RV3, INT, RV3>() { return Sample(RV3(1,2,3), 2, RV3(0.5,1,1.5)); }
        template<> inline Sample<RV3, DBL, RV3> makeSample<RV3, DBL, RV3>() { return Sample(RV3(1,2,3), 2., RV3(0.5,1,1.5)); }
        template<> inline Sample<RV3, BOO, void> makeSample<RV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, STR, void> makeSample<RV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<RV3, MAT3, RV3> makeSample<RV3, MAT3, RV3>() { return Sample(RV3(1,-2,3),(MAT3() << 1,1,0,0,1,1,0,0,1).finished(), RV3(1, -3,  6)); }
        template<> inline Sample<RV3, QUAD, void> makeSample<RV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<CV3, CV3, void> makeSample<CV3, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, RV3, void> makeSample<CV3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, INT, CV3> makeSample<CV3, INT, CV3>() { return Sample(CV3(1,2,3), 2, CV3(0.5,1,1.5)); }
        template<> inline Sample<CV3, DBL, CV3> makeSample<CV3, DBL, CV3>() { return Sample(CV3(1,2,3), 2.0, CV3(0.5,1,1.5)); }
        template<> inline Sample<CV3, BOO, void> makeSample<CV3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, STR, void> makeSample<CV3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, MAT3, void> makeSample<CV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<CV3, QUAD, void> makeSample<CV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<MAT3, MAT3, MAT3> makeSample<MAT3, MAT3, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 1,1,0,0,1,1,0,0,1).finished(), (MAT3() << 1,1,2,4,1,5,7,1,8).finished()); }
        template<> inline Sample<MAT3, INT, MAT3> makeSample<MAT3, INT, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2, (MAT3() << 0.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5).finished()); }
        template<> inline Sample<MAT3, DBL, MAT3> makeSample<MAT3, DBL, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2.0, (MAT3() << 0.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5).finished()); }
        template<> inline Sample<MAT3, BOO, void> makeSample<MAT3, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, STR, void> makeSample<MAT3, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, RV3, void> makeSample<MAT3, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, CV3, void> makeSample<MAT3, CV3, void>() {  throw std::runtime_error("not supported!"); }
        template<> inline Sample<MAT3, QUAD, void> makeSample<MAT3, QUAD, void>() { throw std::runtime_error("not supported!"); }

        template<> inline Sample<QUAD, QUAD, QUAD> makeSample<QUAD, QUAD, QUAD>() { return Sample( QUAD(1, 2, 3, 4),  QUAD(4, 3, 2, 1),  QUAD(0.666666666666667, 0.333333333333333, 0, 0.666666666666667));}
        template<> inline Sample<QUAD, INT, void> makeSample<QUAD, INT, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, DBL, void> makeSample<QUAD, DBL, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, BOO, void> makeSample<QUAD, BOO, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, STR, void> makeSample<QUAD, STR, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, RV3, void> makeSample<QUAD, RV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, CV3, void> makeSample<QUAD, CV3, void>() { throw std::runtime_error("not supported!"); }
        template<> inline Sample<QUAD, MAT3, void> makeSample<QUAD, MAT3, void>() { throw std::runtime_error("not supported!"); }
    };
};