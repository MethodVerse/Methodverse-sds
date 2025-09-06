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

// ---------- Pair wrapper & cartesian product ----------
template<class L, class R, class RES> 
struct Sample { 
    using left_type = L; 
    using right_type = R;
    using result_type = RES; 
    left_type left;
    right_type right;
    result_type result;
    Sample(const L& l, const R& r, const RES& res) : left(l), right(r), result(res) {}
};
template<class L, class R> 
struct Sample<L, R, void> { 
    using left_type = L; 
    using right_type = R;
    using result_type = void; 
    left_type left;
    right_type right;
};

template<class T1, class T2, class T3> 
Sample<T1, T2, T3> makeSample();

using DBL  = double;
using INT  = int;
using BOO  = bool;
using STR  = std::string;
using CV3 = Eigen::Vector3d;
using RV3= Eigen::RowVector3d;
using MAT3 = Eigen::Matrix3d;
using QUAD = Eigen::Quaterniond;

template<class L, class R> struct Pair { using left_type = L; using right_type = R; };

using AllTypes = boost::mp11::mp_list<DBL, INT, BOO, STR, CV3, RV3, MAT3, QUAD>;
using AllPairs    = boost::mp11::mp_product<Pair, AllTypes, AllTypes>;

using AllPairsT = boost::mp11::mp_apply<::testing::Types, AllPairs>;