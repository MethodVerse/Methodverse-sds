// tests/test_primitive_operators1.cpp
#include <gtest/gtest.h>
#include <type_traits>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <gtest/gtest.h>
#include <methodverse/parameter/operation_policy.h>

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

// matrix add
TEST(OpPolicyAdd, MatMat) {
    Eigen::Matrix3d m1 = (Eigen::Matrix3d() << 1, 2, 3, 4, 5, 6, 7, 8, 9).finished();
    Eigen::Matrix3d m2 = (Eigen::Matrix3d() << 9, 8, 7, 6, 5, 4, 3, 2, 1).finished();
    Eigen::Matrix3d ex = (Eigen::Matrix3d() << 10, 10, 10, 10, 10, 10, 10, 10, 10).finished();
    Eigen::Matrix3d r = op_policy<eigen_mat_tag,eigen_mat_tag,add_op>::impl(m1, m2);
    EXPECT_EQ(ex, r);
}

TEST(OpPolicyAllowed, OpPolicyAllowed){
    using Policy = op_policy<eigen_vecmat_tag, scalar_tag, add_op>;
    using Policy1 = op_policy<eigen_vecmat_tag, scalar_tag, coefw_mul_op>;
    static_assert(!Policy1::enabled);
    static_assert(op_allowed<Policy, Eigen::Vector3d, double>);
    static_assert(!op_allowed<Policy, int, double>);
    static_assert(!op_allowed<Policy, float, double>);
    using return_t = op_return_t<Policy, int, double>;
    static_assert(std::is_same_v<void, return_t>);
}