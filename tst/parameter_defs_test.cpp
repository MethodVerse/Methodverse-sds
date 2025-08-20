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

// We will need some real parameter objects to do the test
template<class D> auto make1();
template<> inline auto make1<Tag1Bool>()     { return Tag1Bool{false, true, false}; }
template<> inline auto make1<Tag1Int>()  { return Tag1Int{1,2,3}; }
template<> inline auto make1<Tag1Double>()  { return Tag1Double{1.0, 3.3, 9.1}; }
template<> inline auto make1<Tag1String>()     { return Tag1String{"Hello", "Method", "Verse"}; }
template<> inline auto make1<Tag1EigenVector3d>()     { return Tag1EigenVector3d{Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(4, 5, 6), Eigen::Vector3d(7, 8, 9)}; }
template<> inline auto make1<Tag1EigenMatrix3d>()     { return Tag1EigenMatrix3d{Eigen::Matrix3d::Identity(), Eigen::Matrix3d::Ones(), Eigen::Matrix3d::Zero()}; }
template<> inline auto make1<Tag1EigenQuaterniond>()  { return Tag1EigenQuaterniond{Eigen::Quaterniond(1, 0, 0, 0), Eigen::Quaterniond(2, 0, 0, 0), Eigen::Quaterniond(3, 0, 0, 0)}; }

template<class D> auto make2();
template<> inline auto make2<Tag2Bool>()     { return Tag2Bool{false, true, false}; }
template<> inline auto make2<Tag2Int>()  { return Tag2Int{1,2,3}; }
template<> inline auto make2<Tag2Double>()  { return Tag2Double{1.0, 3.3, 9.1}; }
template<> inline auto make2<Tag2String>()     { return Tag2String{"Hello", "Method", "Verse"}; }
template<> inline auto make2<Tag2EigenVector3d>()     { return Tag2EigenVector3d{Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(4, 5, 6), Eigen::Vector3d(7, 8, 9)}; }
template<> inline auto make2<Tag2EigenMatrix3d>()     { return Tag2EigenMatrix3d{Eigen::Matrix3d::Identity(), Eigen::Matrix3d::Ones(), Eigen::Matrix3d::Zero()}; }
template<> inline auto make2<Tag2EigenQuaterniond>()  { return Tag2EigenQuaterniond{Eigen::Quaterniond(1, 0, 0, 0), Eigen::Quaterniond(2, 0, 0, 0), Eigen::Quaterniond(3, 0, 0, 0)}; }

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
template<class PairType>
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

    // 5) constuct and assign from primitive type value
    using Primitive = typename L::value_type;
    Primitive whatever = static_cast<Primitive>(e[0]); // get the first value from e
    L a5(whatever); // construct from primitive type value
    EXPECT_EQ(e[0], a5[0]); // check the first value
    EXPECT_EQ(1, a5.Get().size()); // should only have one value

    a5 = e;
    a5 = static_cast<Primitive>(e[1]);
    EXPECT_EQ(e[1], a5[0]); // check the first value
    EXPECT_EQ(1, a5.Get().size()); // should only have one value

    // 6) construct and assign from primitive type vector
    auto vec = e.ToVector();
    L a6(vec); // construct from primitive type value
    EXPECT_EQ(e, a6); // should be equal to e

    a6.Get().clear(); // clear the value
    a6 = vec;
    EXPECT_EQ(e, a6); // should be equal to e

    // 7) construct and assign from initializer list
    L a7{e[0], e[1], e[2]}; // construct from initializer list
    EXPECT_EQ(e, a7); // should be equal to e
    a7.Get().clear(); // clear the value
    a7 = {e[0], e[1], e[2]}; // assign from initializer list
    EXPECT_EQ(e, a7); // should be equal to e
}

TYPED_TEST(CrossTagTest, ConversionOperatorsToScalarAndVector) {
    using L = TypeParam::Lhs;

    // Establish a reference expected value for comparison
    const L e = make1<L>();

    // 1) conversion to primitive type
    auto s = e.ToScalar();
    EXPECT_EQ(e[0], s); // should be equal to the first value of e
    auto v = e.ToVector();
    EXPECT_EQ(e.Get(), v); // should be equal to the vector representation of e
}

TYPED_TEST(CrossTagTest, AccessOperator) {
    using L = TypeParam::Lhs;
    using R = TypeParam::Rhs;

    L a = make1<L>();
    R b = make2<R>();

    a[0] = b[0]; 
    EXPECT_EQ(a[0], b[0]);
    a[1] = b[1];
    EXPECT_EQ(a[1], b[1]);
    a[2] = b[2];
    EXPECT_EQ(a[2], b[2]); 
}

TYPED_TEST(CrossTagTest, ParameterNameUnit) {
    using L = TypeParam::Lhs;

    L a = make1<L>();

    EXPECT_EQ(std::string(L::name), a.Name());
    EXPECT_EQ(typeid(L), a.Type());
    EXPECT_EQ(std::string(L::unit), a.Unit());
}

TYPED_TEST(CrossTagTest, ValueAsString) {
    using L = TypeParam::Lhs;

    L a = make1<L>();

    // tentative test: just make sure calling ValueAsString() does not throw and returns a non-empty string
    EXPECT_NO_THROW({
        auto s = a.ValueAsString();
        EXPECT_FALSE(s.empty());
    });
    EXPECT_EQ(a.ValueAsString(), a.ValueAsString());
}

TYPED_TEST(CrossTagTest, GetterAndSetter) {
    using L = TypeParam::Lhs;

    L e = make1<L>();
    L a;
    a.Set(e[0]);
    EXPECT_EQ(1, a.Get().size());
    EXPECT_EQ(e[0], a[0]);

    a.Set(e.ToVector());
    EXPECT_EQ(e, a);

    a.Set({e[0], e[1], e[2]});
    EXPECT_EQ(e, a);
    
    EXPECT_EQ(a.Get()[0], e[0]);
    EXPECT_EQ(a.Get()[1], e[1]);    
    EXPECT_EQ(a.Get()[2], e[2]);
}

TYPED_TEST(CrossTagTest, EqualityOperator) {
    using L = TypeParam::Lhs;

    L a = make1<L>();
    L b = make1<L>();

    EXPECT_EQ(a, b); // should be equal
}

TYPED_TEST(CrossTagTest, ElementWiseBinaryOperationOfSameTag) {
    using L = TypeParam::Lhs;
    using T = typename L::value_type;
    L a = make1<L>();
    L b = make1<L>();

    static_assert(std::is_same_v<category_t<int>, scalar_tag>, "int should be a scalar type");
    static_assert(std::is_same_v<category_t<double>, scalar_tag>, "double should be a scalar type");
    static_assert(std::is_same_v<category_t<std::string>, string_tag>, "std::string should be a string type");
    static_assert(std::is_same_v<category_t<bool>, bool_tag>, "Eigen::VectorXd should be a bool type");
    static_assert(std::is_same_v<category_t<Eigen::Vector3d>, eigen_vec_tag>, "Eigen::Vector3d should be a vector type");
    static_assert(std::is_same_v<category_t<Eigen::Matrix3d>, eigen_mat_tag>, "Eigen::Matrix3d should be a matrix type");
    static_assert(std::is_same_v<category_t<Eigen::Quaterniond>, eigen_quat_tag>, "Eigen::Quaterniond should be a quaternion type");
    // if constexpr (SelfAddable<typename L::value_type>) {
    //     // Test addition
    //     auto add_result = elementWiseBinaryOp(a, b, [](auto x, auto y) { return x + y; });
    //     EXPECT_EQ(add_result, a+b);
    // }
    // else {
    //     GTEST_SKIP() << "Skipping addition test for non-addable type"
    //                  << typeid(L).name();
    // }

    // if constexpr (SelfSubtractable<typename L::value_type>) {
    //     // Test addition
    //     auto subtract_result = elementWiseBinaryOp(a, b, [](auto x, auto y) { return x - y; });
    //     EXPECT_EQ(subtract_result, a-b);
    // }
    // else {
    //     GTEST_SKIP() << "Skipping addition test for non-addable type"
    //                  << typeid(L).name();
    // }

    // if constexpr (SelfMultipliable<typename L::value_type>) {
    //     // Test multiplication
    //     auto multiply_result = elementWiseBinaryOp(a, b, [](auto x, auto y) { return x * y; });
    //     EXPECT_EQ(multiply_result, a*b);
    // }
    // else {
    //     GTEST_SKIP() << "Skipping multiplication test for non-multipliable type"
    //                  << typeid(L).name();
    // }

    // if constexpr (SelfDividable<typename L::value_type>) {
    //     // Test division
    //     auto divide_result = elementWiseBinaryOp(a, b, [](auto x, auto y) { return x / y; });
    //     EXPECT_EQ(divide_result, a/b);
    // }
    // else {
    //     GTEST_SKIP() << "Skipping division test for non-dividable type"
    //                  << typeid(L).name();
    // }
}