// tests/test_primitive_operators1.cpp
#include <gtest/gtest.h>
#include <type_traits>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <gtest/gtest.h>
#include <methodverse/parameter/operation_policy.h>
#include <methodverse/parameter/parameter.h>

using namespace methodverse::parameter;

// scalar + scalar
TEST(OpPolicyAdd, ScalarScalar) {
    double r = op_policy<scalar_tag,scalar_tag,add_op>::impl(2, 3.5);
    EXPECT_DOUBLE_EQ(r, 5.5);
    static_assert(std::is_same_v<
        decltype(op_policy<scalar_tag,scalar_tag,add_op>::impl(1,2)),
        int>);
}

// scalar + vector
TEST(OpPolicyAdd, ScalarVector) {
    Eigen::Vector3d v(1,2,3);
    Eigen::Vector3d r = op_policy<scalar_tag,eigen_colvec_tag,add_op>::impl(2.0, v);
    EXPECT_EQ(r, Eigen::Vector3d(3,4,5));

    auto x = op_policy<scalar_tag,eigen_rowvec_tag,add_op>::impl(2.0, v.transpose().eval());
    EXPECT_EQ(x, Eigen::RowVector3d(3,4,5));
}

// vector + scalar (delegates)
TEST(OpPolicyAdd, VectorScalar) {
    Eigen::Vector3d v(1,2,3);
    Eigen::Vector3d r = op_policy<eigen_colvec_tag,scalar_tag,add_op>::impl(v, 2.0);
    EXPECT_EQ(r, Eigen::Vector3d(3,4,5));
}

// quaternion add
TEST(OpPolicyAdd, QuatQuat) {
    Eigen::Quaterniond q1(1,0,0,0), q2(1,1,1,1);
    Eigen::Quaterniond r = op_policy<eigen_quat_tag,eigen_quat_tag,add_op>::impl(q1,q2);
    EXPECT_EQ(r.coeffs(), q1.coeffs() + q2.coeffs());
}

// vector / scalar
TEST(OpPolicyDiv, VectorScalar) {
    Eigen::Vector3d v(2,4,6);
    Eigen::Vector3d r = op_policy<eigen_colvec_tag,scalar_tag,div_op>::impl(v,2.0);
    EXPECT_EQ(r, Eigen::Vector3d(1,2,3));
}

