#include <gtest/gtest.h>
#include "parameter.h"

// Sample enums for testing setEnum/getEnum
enum class ProSat { Off = 0, On = 1 };
enum class ScanMode { ModeA = 0, ModeB, ModeC };

// Test the following member functions:
//   template <typename T> Parameter(T a);            => initialization with primitive types
//   template <typename T> Parameter &operator=(T a); => assignment from primitive types
//   template <typename T> operator T() const;        => conversion to primitive types
//   getValue() const;                                => getValue() returns the variant value of the parameter
TEST(ParameterTest, InitializesWithPrimitiveTypes) {
    //
    Parameter p_int(42);
    int x_int = p_int;
    p_int = x_int;
    EXPECT_EQ(42, x_int);
    EXPECT_EQ(42, std::get<int>(p_int.getValue()));

    Parameter p_double(3.14);
    double x_double = p_double;
    p_double = x_double;
    EXPECT_DOUBLE_EQ(3.14, x_double);
    EXPECT_DOUBLE_EQ(3.14, std::get<double>(p_double.getValue()));

    Parameter p_bool(true);
    bool x_bool = p_bool;
    p_bool = x_bool;
    EXPECT_EQ(true, x_bool);
    EXPECT_EQ(true, std::get<bool>(p_bool.getValue()));

    Parameter p_string(std::string("hello"));
    std::string x_string = p_string;
    p_string = x_string;
    EXPECT_EQ("hello", x_string);
    EXPECT_EQ("hello", std::get<std::string>(p_string.getValue()));

    Parameter p_vec3(Eigen::Vector3d(1, 2, 3));
    Eigen::Vector3d x_vec3 = p_vec3;
    p_vec3 = x_vec3;
    EXPECT_EQ(1, x_vec3(0));
    EXPECT_EQ(2, x_vec3(1));
    EXPECT_EQ(3, x_vec3(2));
    EXPECT_EQ(1, std::get<Eigen::Vector3d>(p_vec3.getValue())(0));
    EXPECT_EQ(2, std::get<Eigen::Vector3d>(p_vec3.getValue())(1));
    EXPECT_EQ(3, std::get<Eigen::Vector3d>(p_vec3.getValue())(2));

    Eigen::Matrix3d y_mat3 = Eigen::Matrix3d::Identity();
    Parameter p_mat3(y_mat3);
    Eigen::Matrix3d x_mat3 = p_mat3;
    p_mat3 = x_mat3;
    EXPECT_EQ(1, x_mat3(0, 0));
    EXPECT_EQ(1, x_mat3(1, 1));
    EXPECT_EQ(1, x_mat3(2, 2));
    EXPECT_EQ(1, std::get<Eigen::Matrix3d>(p_mat3.getValue())(0, 0));
    EXPECT_EQ(1, std::get<Eigen::Matrix3d>(p_mat3.getValue())(1, 1));
    EXPECT_EQ(1, std::get<Eigen::Matrix3d>(p_mat3.getValue())(2, 2));

    Parameter p_quat(Eigen::Quaterniond(1, 0, 0, 0));
    Eigen::Quaterniond x_quat = p_quat;
    p_quat = x_quat;
    EXPECT_DOUBLE_EQ(1, x_quat.w());
    EXPECT_DOUBLE_EQ(0, x_quat.x());
    EXPECT_DOUBLE_EQ(0, x_quat.y());
    EXPECT_DOUBLE_EQ(0, x_quat.z());
    EXPECT_DOUBLE_EQ(1, std::get<Eigen::Quaterniond>(p_quat.getValue()).w());
    EXPECT_DOUBLE_EQ(0, std::get<Eigen::Quaterniond>(p_quat.getValue()).x());
    EXPECT_DOUBLE_EQ(0, std::get<Eigen::Quaterniond>(p_quat.getValue()).y());
    EXPECT_DOUBLE_EQ(0, std::get<Eigen::Quaterniond>(p_quat.getValue()).z());

    std::vector<Eigen::Matrix3d> vec_mat3 = {Eigen::Matrix3d::Identity(), Eigen::Matrix3d::Identity()};
    Parameter p_vec_mat3(vec_mat3);
    std::vector<Eigen::Matrix3d> x_vec_mat3 = p_vec_mat3;
    p_vec_mat3 = x_vec_mat3;
    EXPECT_EQ(2, x_vec_mat3.size());
    EXPECT_EQ(1, x_vec_mat3[0](0, 0));
    EXPECT_EQ(1, x_vec_mat3[0](1, 1));
    EXPECT_EQ(1, x_vec_mat3[0](2, 2));
    EXPECT_EQ(1, x_vec_mat3[1](0, 0));
    EXPECT_EQ(1, x_vec_mat3[1](1, 1));
    EXPECT_EQ(1, x_vec_mat3[1](2, 2));
}

// Test the following member functions:
//   copy constructor
//   move constructor
//   copy assignment operator
//   move assignment operator
//   operator==
//   operator!=
TEST(ParameterTest, CopyAndMoveConstructors) {
    // Ues the heaviest type for testing
    std::vector<Eigen::Matrix3d> vec_mat3 = {Eigen::Matrix3d::Identity(), Eigen::Matrix3d::Identity()};
    Parameter::ValueType variant_value = vec_mat3;

    Parameter original;
    original.setValue(variant_value);

    // Copy constructor
    Parameter copy(original);
    EXPECT_EQ(original, copy);

    // Move constructor
    Parameter backup(original);
    Parameter moved(std::move(original));
    EXPECT_EQ(backup, moved);
    EXPECT_TRUE(moved == original || moved != original); // moved can either equal or not equal original after move operation

    // Copy assignment operator
    original = backup;
    EXPECT_EQ(backup, original);

    // Move assignment operator
    original = std::move(moved);
    EXPECT_EQ(backup, original);
    EXPECT_TRUE(original == moved || original != moved); // original can either equal or not equal to moved after its resource is moved

    variant_value = std::vector<Eigen::Matrix3d>{Eigen::Matrix3d::Zero(), Eigen::Matrix3d::Identity()}; // Change the second matrix to zero
    original.setValue(variant_value);
    EXPECT_NE(original, backup);
    EXPECT_TRUE(original != backup);

}

bool vectorsApproxEqual(const std::vector<Eigen::Matrix3d>& a, const std::vector<Eigen::Matrix3d>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (!a[i].isApprox(b[i])) return false;
    }
    return true;
}

// Test the following member functions:
//   setValue
//   getValue
TEST(ParameterTest, SetAndGetValue) {
    Parameter p;
    Parameter::ValueType value = std::vector<Eigen::Matrix3d>{Eigen::Matrix3d::Identity(), Eigen::Matrix3d::Identity()};
    p.setValue(value);
    EXPECT_TRUE(vectorsApproxEqual(std::get<std::vector<Eigen::Matrix3d>>(value), std::get<std::vector<Eigen::Matrix3d>>(p.getValue())));
}

// Test the following memeber functions:
//   setEnum
//   getEnum
TEST(ParameterTest, SetAndGetEnum) {
    Parameter p;
    p.setEnum(ProSat::On);
    EXPECT_EQ(ProSat::On, p.getEnum<ProSat>());         
    p.setEnum(ScanMode::ModeB);
    EXPECT_EQ(ScanMode::ModeB, p.getEnum<ScanMode>());
    // Check that the type is preserved
    EXPECT_THROW(p.getEnum<ProSat>(), std::runtime_error); // Should throw because the type is not ProSat
    EXPECT_NO_THROW(p.setEnum(ProSat::Off)); // Should not throw because you can set the param to another enum
    EXPECT_EQ(ProSat::Off, p.getEnum<ProSat>()); // Should be able to get ProSat after setting it
}