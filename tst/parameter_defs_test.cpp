#include <gtest/gtest.h>
#include "parameter_defs.h"
#include <Eigen/Dense>

using namespace mv;
// 

DEFINE_TYPED_PARAMETER(TE, "Echo Time", double, "ms")
DEFINE_TYPED_PARAMETER(TR, "Repetition Time", double, "ms")
DEFINE_TYPED_PARAMETER(FlipAngle, "Flip Angle", double, "deg")
DEFINE_TYPED_PARAMETER(Rotation, "Rotation", Eigen::Matrix3d, "")

class ParameterTest : public ::testing::Test {
protected:
    TE te_param;
    TR tr_param;
    FlipAngle fa_param;
    Rotation rot_param;

    void SetUp() override {
        te_param = TE({30.0});
        tr_param = TR({1000.0});
        fa_param = FlipAngle({90.0});
        rot_param = Rotation(Eigen::Matrix3d::Identity());
    }
};