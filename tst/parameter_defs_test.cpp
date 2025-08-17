#include <gtest/gtest.h>
#include "parameter_defs.h"
#include <Eigen/Dense>

using namespace mv;

// Define some concrete parameter types for each primitive type, 
// Note that two tags are defined fro each primitive type, to test corss operation on different tags but with same primitive type.
DEFINE_TYPED_PARAMETER(Tag1Bool,    "Tag1Bool",    bool, "")
DEFINE_TYPED_PARAMETER(Tag2Bool,    "Tag2Bool",    bool, "")

DEFINE_TYPED_PARAMETER(Tag1Int,     "Tag1Int",     int, "")
DEFINE_TYPED_PARAMETER(Tag2Int,     "Tag2Int",     int, "")

DEFINE_TYPED_PARAMETER(Tag1Double,  "Tag1Double",  double, "")
DEFINE_TYPED_PARAMETER(Tag2Double,  "Tag2Double",  double, "")

DEFINE_TYPED_PARAMETER(Tag1String,  "Tag1String",  std::string, "")
DEFINE_TYPED_PARAMETER(Tag2String,  "Tag2String",  std::string, "")

DEFINE_TYPED_PARAMETER(Tag1EigenVector3d,     "Tag1EigenVector3d", Eigen::Vector3d, "")
DEFINE_TYPED_PARAMETER(Tag2EigenVector3d,     "Tag2EigenVector3d", Eigen::Vector3d, "")

DEFINE_TYPED_PARAMETER(Tag1EigenMatrix3d,     "Tag1EigenMatrix3d", Eigen::Matrix3d, "")
DEFINE_TYPED_PARAMETER(Tag2EigenMatrix3d,     "Tag2EigenMatrix3d", Eigen::Matrix3d, "")

DEFINE_TYPED_PARAMETER(Tag1EigenQuaterniond,  "Tag1EigenQuaterniond", Eigen::Quaterniond, "")
DEFINE_TYPED_PARAMETER(Tag2EigenQuaterniond,  "Tag2EigenQuaterniond", Eigen::Quaterniond, "")

// // Repeat the definitions for the vector primitive types
// DEFINE_TYPED_PARAMETER(Tag1BoolVec,    "Tag1BoolVec",    std::vector<bool>, "")
// DEFINE_TYPED_PARAMETER(Tag2BoolVec,    "Tag2BoolVec",    std::vector<bool>, "")

// DEFINE_TYPED_PARAMETER(Tag1IntVec,     "Tag1IntVec",     std::vector<int>, "")
// DEFINE_TYPED_PARAMETER(Tag2IntVec,     "Tag2IntVec",     std::vector<int>, "")

// DEFINE_TYPED_PARAMETER(Tag1DoubleVec,  "Tag1DoubleVec",  std::vector<double>, "")
// DEFINE_TYPED_PARAMETER(Tag2DoubleVec,  "Tag2DoubleVec",  std::vector<double>, "")

// DEFINE_TYPED_PARAMETER(Tag1StringVec,  "Tag1StringVec",  std::vector<std::string>, "")
// DEFINE_TYPED_PARAMETER(Tag2StringVec,  "Tag2StringVec",  std::vector<std::string>, "")

// DEFINE_TYPED_PARAMETER(Tag1EigenVector3dVec,     "TagEigenVector3dVec", std::vector<Eigen::Vector3d>, "")
// DEFINE_TYPED_PARAMETER(Tag2EigenVector3dVec,     "TagEigenVector3dVec", std::vector<Eigen::Vector3d>, "")

// DEFINE_TYPED_PARAMETER(Tag1EigenMatrix3dVec,     "TagEigenMatrix3dVec", std::vector<Eigen::Matrix3d>, "")
// DEFINE_TYPED_PARAMETER(Tag2EigenMatrix3dVec,     "TagEigenMatrix3dVec", std::vector<Eigen::Matrix3d>, "")

// DEFINE_TYPED_PARAMETER(Tag1EigenQuaterniondVec,  "TagEigenQuaterniondVec", std::vector<Eigen::Quaterniond>, "")
// DEFINE_TYPED_PARAMETER(Tag2EigenQuaterniondVec,  "TagEigenQuaterniondVec", std::vector<Eigen::Quaterniond>, "")

// We will need some real parameter objects to do the test
template<class D> auto make1();
template<> inline auto make1<Tag1Bool>()     { return Tag1Bool{false, true, false}; }
template<> inline auto make1<Tag1Int>()  { return Tag1Int{1,2,3}; }
template<> inline auto make1<Tag1Double>()  { return Tag1Double{1.0, 3.3, 9.1}; }
template<> inline auto make1<Tag1String>()     { return Tag1String{"Hello", "Method", "Verse"}; }
template<> inline auto make1<Tag1EigenVector3d>()     { return Tag1EigenVector3d{Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(4, 5, 6), Eigen::Vector3d(7, 8, 9)}; }
template<> inline auto make1<Tag1EigenMatrix3d>()     { return Tag1EigenMatrix3d{Eigen::Matrix3d::Identity(), Eigen::Matrix3d::Ones(), Eigen::Matrix3d::Zero()}; }
template<> inline auto make1<Tag1EigenQuaterniond>()  { return Tag1EigenQuaterniond{Eigen::Quaterniond(1, 0, 0, 0), Eigen::Quaterniond(1, 0, 0, 0)}; }

template<class D> auto make2();
template<> inline auto make2<Tag2Bool>()     { return Tag2Bool{false, true, false}; }
template<> inline auto make2<Tag2Int>()  { return Tag2Int{1,2,3}; }
template<> inline auto make2<Tag2Double>()  { return Tag2Double{1.0, 3.3, 9.1}; }
template<> inline auto make2<Tag2String>()     { return Tag2String{"Hello", "Method", "Verse"}; }
template<> inline auto make2<Tag2EigenVector3d>()     { return Tag2EigenVector3d{Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(4, 5, 6), Eigen::Vector3d(7, 8, 9)}; }
template<> inline auto make2<Tag2EigenMatrix3d>()     { return Tag2EigenMatrix3d{Eigen::Matrix3d::Identity(), Eigen::Matrix3d::Ones(), Eigen::Matrix3d::Zero()}; }
template<> inline auto make2<Tag2EigenQuaterniond>()  { return Tag2EigenQuaterniond{Eigen::Quaterniond(1, 0, 0, 0), Eigen::Quaterniond(1, 0, 0, 0)}; }

// Helper definition to organize two tags into one type, which is added to the type list for google typed tests.
template<class L, class R>
struct Pair { using Lhs = L; using Rhs = R; };

using CrossTypes = ::testing::Types<
    Pair<Tag1Bool,    Tag2Bool>,
    Pair<Tag1Int,     Tag2Int>,
    Pair<Tag1Double,  Tag2Double>,
    Pair<Tag1String,  Tag2String>,
    Pair<Tag1EigenVector3d,     Tag2EigenVector3d>,
    Pair<Tag1EigenMatrix3d,     Tag2EigenMatrix3d>,
    Pair<Tag1EigenQuaterniond,  Tag2EigenQuaterniond>
    // ...
>;

// Define a test fixture for google typed test framework
template<class P>
class CrossTagTest : public ::testing::Test {};
TYPED_TEST_SUITE(CrossTagTest, CrossTypes);

// Define test for construct, copy and move operations
TYPED_TEST(CrossTagTest, ConstructCopyMove) {
    using L = TypeParam::Lhs;

    static_assert(std::is_default_constructible_v<L>);
    static_assert(std::is_copy_constructible_v<L>);
    static_assert(std::is_move_constructible_v<L>);
    static_assert(std::is_copy_assignable_v<L>);
    static_assert(std::is_move_assignable_v<L>);
    static_assert(std::is_nothrow_move_constructible_v<L>);
    static_assert(std::is_nothrow_move_assignable_v<L>);  

    // Establish a reference expected value for comparison
    const L e = make1<L>();

    // 0) default constructor
    if constexpr (std::is_default_constructible_v<L>){
        L dflt{};
        EXPECT_EQ(0, dflt.Get().size()); // default constructed object should not be equal to e
    }
    
    // 1) copy constructor
    L a1(e);
    EXPECT_EQ(e, a1);

    // 2) move constructor
    L a2(std::move(a1));
    EXPECT_EQ(e, a2);

    // 3) copy assignment
    L a3 = e;         // copy-assign
    EXPECT_EQ(e, a3);

    // 4) move assignment
    L a4 = std::move(a3); // start with a different value
    EXPECT_EQ(e, a4);

    // 5) default constructor
    L a5; // default-construct


}