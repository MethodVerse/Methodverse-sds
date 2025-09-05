#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <mp-units/systems/si.h>
#include <iostream>
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <methodverse/parameter/parameter.h>
// #define MP_UNITS_USE_FMTLIB 1

using namespace mv;
using namespace mp_units;

// Define factory methods to return primitive values for different parameter types
// Coveats: we have to define methods for all possible primitie types used in the tests, in Ts;
//          we also fixed the number of values (3) to be returned for each type.
template<class D> std::vector<D> make();
template<> inline std::vector<bool> make<bool>() { 
    return std::vector{false, true, false}; 
}
template<> inline std::vector<int> make<int>()  { 
    return std::vector{1,2,3}; 
}
template<> inline std::vector<double> make<double>()  { 
    return std::vector{1.0, 3.3, 9.1}; 
}
template<> inline std::vector<std::string> make<std::string>() { 
    return std::vector{std::string("Hello"), std::string("Method"), std::string("Verse")}; 
}
template<> inline std::vector<Eigen::Vector3d> make<Eigen::Vector3d>() { 
    return std::vector{Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(4, 5, 6), Eigen::Vector3d(7, 8, 9)}; 
}
template<> inline std::vector<Eigen::Matrix3d> make<Eigen::Matrix3d>() { 
    return std::vector{(1.0*Eigen::Matrix3d::Identity()).eval(), (2.0*Eigen::Matrix3d::Identity()).eval(), (3.0*Eigen::Matrix3d::Identity()).eval()}; 
}
template<> inline std::vector<Eigen::Quaterniond> make<Eigen::Quaterniond>()  { 
    return std::vector{Eigen::Quaterniond(1, 0, 0, 0), Eigen::Quaterniond(2, 0, 0, 0), Eigen::Quaterniond(3, 0, 0, 0)}; 
}

// Define a dummy parameter class for testing
template<class T, class Derived, auto U>
struct ParamCRTP : public ParameterBase<T, U> {
    using ParameterBase<T, U>::ParameterBase;
    static constexpr const char* name = "ParamCRTP";
};

template<class T, auto U>
class Param: public ParamCRTP<T, Param<T, U>, U> {
public:
    using Base = ParamCRTP<T, Param<T,U>, U>;
    using Base::Base;
    static constexpr const char* name = "Param";
    std::string Name() const override {
        return name;
    }
};

// Helpper to convert a value to type 
template<auto U>
using UnitC = std::integral_constant<decltype(U), U>;

// Define the types and units to be tested, and the resulting Param<T,U> types
using Ts    = boost::mp11::mp_list<double, int, bool, std::string, Eigen::Vector3d, Eigen::Matrix3d, Eigen::Quaterniond>;
using Units = boost::mp11::mp_list<UnitC<si::metre>, UnitC<si::second>, UnitC<si::tesla>>;

template<class T, class U>
using MakeParam = Param<T, U::value>;

// Take an mp_list<T,U> and build MakeParam<T,U>
template<class Pair>
using ApplyMakeParam = MakeParam<boost::mp11::mp_at_c<Pair, 0>, boost::mp11::mp_at_c<Pair, 1>>;

// Cartesian product of Ts × Units -> list of Param<T,U>
// Build the cartesian product of two mp_lists
// boost::mp11::mp_product<boost::mp11::mp_list, Ts, Units> yields this:
// mp_list<
//     mp_list<double, UnitC<metre>>,
//     mp_list<double, UnitC<second>>,
//     mp_list<double, UnitC<tesla>>,
//     mp_list<int,    UnitC<metre>>,
//     mp_list<int,    UnitC<second>>,
//     mp_list<int,    UnitC<tesla>>
// >
// boost::mp11::mp_quote<ApplyMakeParam>:
// is a metafunction class that applies ApplyMakeParam to each element of the above list, and yields:
// mp_list<
//     MakeParam<T1,U1> = Param<T1,U1::value>,
//     MakeParam<T1,U2> = Param<T1,U2::value>,
//     MakeParam<T1,U3> = Param<T1,U3::value>,
//     MakeParam<T2,U1> = Param<T2,U1::value>,
//     MakeParam<T2,U2> = Param<T2,U2::value>,
//     MakeParam<T2,U3> = Param<T2,U3::value>
// > // which is a list or Param<T,U> spaning all combinations of T and U
using ParamTypes = boost::mp11::mp_transform_q<
    boost::mp11::mp_quote<ApplyMakeParam>,
    boost::mp11::mp_product<boost::mp11::mp_list, Ts, Units>>;

using GTestTypes = boost::mp11::mp_apply<::testing::Types, ParamTypes>;

template<class P>
class ParameterBaseTypedTest : public ::testing::Test { }; 

// helper function to print the type pack at compile time
template<class... Ts>
void debug_pack_indexed() {
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

TYPED_TEST_SUITE(ParameterBaseTypedTest, GTestTypes);

TYPED_TEST(ParameterBaseTypedTest, CanConstructAndUse) {

    // Establish some type aliases for convenience, and a Param object (pe) as reference
    using ParameterType = TypeParam;
    using T = typename ParameterType::value_type;
    auto U = ParameterType::GetUnit();

    auto primitive_values = make<T>();
    Param<T, U> pe(primitive_values);
    EXPECT_EQ(primitive_values.size(), pe.Size());
    

    // 0) Default constructor: object should have size 0
    ParameterType p0;
    EXPECT_EQ(0, p0.Size());
    EXPECT_EQ("Param", p0.Name());

    // 1) copy constructor
    ParameterType p1(pe);
    EXPECT_EQ(pe, p1);

    // 2) move constructor
    ParameterType p2(std::move(p1));
    EXPECT_EQ(pe, p2);

    // 3) copy assignment
    ParameterType p3 = pe;
    EXPECT_EQ(pe, p3);

    // 4) move assignment
    ParameterType p4 = std::move(p3); 
    EXPECT_EQ(pe, p4);

    // 5) constuct and assign from a primitive type scalar value   
    T s = primitive_values[0];
    ParameterType p5(s);
    EXPECT_EQ(pe[0], p5[0]);
    EXPECT_EQ(1, p5.Get().size());

    p5 = pe;
    p5 = static_cast<T>(pe[1]);
    EXPECT_EQ(pe[1], p5[0]);
    EXPECT_EQ(1, p5.Get().size());

    // 6) construct and assign from primitive type vector
    ParameterType p6(primitive_values);
    EXPECT_EQ(pe, p6);

    p6.Get().clear();
    p6 = primitive_values;
    EXPECT_EQ(pe, p6);

    // 7) construct and assign from initializer list
    ParameterType p7{pe[0], pe[1], pe[2]};
    EXPECT_EQ(pe, p7);
    p7.Get().clear();
    p7 = {pe[0], pe[1], pe[2]};
    EXPECT_EQ(pe, p7); 
}

TYPED_TEST(ParameterBaseTypedTest, ConversionOperatorsToScalarAndVector) {

    // Establish some type aliases for convenience, and a Param object (pe) as reference
    using ParameterType = TypeParam;
    using T = typename ParameterType::value_type;
    auto U = ParameterType::GetUnit();

    auto primitive_values = make<T>();
    Param<T, U> pe(primitive_values);
    EXPECT_EQ(primitive_values.size(), pe.Size());

    // 1) conversion to primitive scalar
    auto s = pe.Val();
    EXPECT_EQ(pe[0], s); // should be equal to the first value of e
    
    // 2) conversion to primitive vector
    auto v = pe.Vals();
    EXPECT_EQ(pe.Get(), v); // should be equal to the vector representation of e
}

TYPED_TEST(ParameterBaseTypedTest, AccessOperator) {

    // Establish some type aliases for convenience, and a Param object (pe) as reference
    using ParameterType = TypeParam;
    using T = typename ParameterType::value_type;
    auto U = ParameterType::GetUnit();

    auto primitive_values = make<T>();
    Param<T, U> pe(primitive_values);
    
    Param<T, U> p1;
    p1.Get().resize(3);

    p1[0] = primitive_values[0]; 
    EXPECT_EQ(primitive_values[0], p1[0]);
    p1[1] = primitive_values[1];
    EXPECT_EQ(primitive_values[1], p1[1]);
    p1[2] = primitive_values[2];
    EXPECT_EQ(primitive_values[2], p1[2]); 
    EXPECT_EQ(pe, p1);
}

TYPED_TEST(ParameterBaseTypedTest, ParameterNameUnit) {

    // Establish some type aliases for convenience, and a Param object (pe) as reference
    using ParameterType = TypeParam;
    using T = typename ParameterType::value_type;
    auto U = ParameterType::GetUnit();

    auto primitive_values = make<T>();
    Param<T, U> pe(primitive_values);

    EXPECT_EQ(std::string("Param"), pe.Name());
    static_assert(U == ParameterType::GetUnit());
}

TYPED_TEST(ParameterBaseTypedTest, ValueAsString) {

    // Establish some type aliases for convenience, and a Param object (pe) as reference
    using ParameterType = TypeParam;
    using T = typename ParameterType::value_type;
    auto U = ParameterType::GetUnit();

    auto primitive_values = make<T>();
    Param<T, U> pe(primitive_values);

    // tentative test: just make sure calling ValueAsString() does not throw and returns a non-empty string
    EXPECT_NO_THROW({
        auto s = pe.ValueAsString();
        EXPECT_FALSE(s.empty());
    });

}

TYPED_TEST(ParameterBaseTypedTest, GetterAndSetter) {

    // Establish some type aliases for convenience, and a Param object (pe) as reference
    using ParameterType = TypeParam;
    using T = typename ParameterType::value_type;
    auto U = ParameterType::GetUnit();

    auto primitive_values = make<T>();
    Param<T, U> pe(primitive_values);

    ParameterType p0;
    p0.Set(pe[0]);
    EXPECT_EQ(1, p0.Get().size());
    EXPECT_EQ(pe[0], p0[0]);

    p0.Set(pe.Vals());
    EXPECT_EQ(pe, p0);

    p0.Set({pe[0], pe[1], pe[2]});
    EXPECT_EQ(pe, p0);
    
    EXPECT_EQ(pe[0], p0.Get()[0]);
    EXPECT_EQ(pe[1], p0.Get()[1]);    
    EXPECT_EQ(pe[2], p0.Get()[2]);
}

TYPED_TEST(ParameterBaseTypedTest, EqualityOperator) {
    // Establish some type aliases for convenience, and a Param object (pe) as reference
    using ParameterType = TypeParam;
    using T = typename ParameterType::value_type;
    auto U = ParameterType::GetUnit();

    auto primitive_values = make<T>();
    Param<T, U> pe(primitive_values);
    Param<T, U> p1(primitive_values);

    EXPECT_EQ(pe, p1);
}


TEST(OpPolicyAdd, ScalarScalar) {
    Param<double, si::metre> p1(2.0);
    Param<double, si::metre> p2(3.5);
    auto r = p1 + p2;
    EXPECT_DOUBLE_EQ(r.Val(), 5.5);
    static_assert(std::is_same_v<decltype(r), ParameterBase<double, si::metre>>);
}

TEST(OpPolicyMul, ScalarScalarResultantValuAndUnitCorrect) {
    constexpr auto Hz_per_T = si::hertz / si::tesla;
    constexpr auto T_per_m = si::tesla / si::metre;
    
    Param<double, Hz_per_T> gamma(42.577478461e6);
    Param<double, T_per_m> grad_str(10.0);
    Param<double, si::second> dt(0.001);

    auto gamma_grad = gamma * grad_str; // should be of unit hertz/metre
    //EXPECT_DOUBLE_EQ(gamma_grad.Val(), 425774.78461);
    auto r = gamma * grad_str * dt; // should be of unit hertz/metre * tesla/metre * second = hertz/metre
    EXPECT_DOUBLE_EQ(r.Val(), 425774.78461);
    //using ExpectedType = ParameterBase<double, si::hertz / si::metre / si::second>;
    static_assert(si::hertz / si::metre * si::second == decltype(r)::GetUnit(), "Unit should be hertz/metre/second");
    std::cout << "r unit: " << decltype(r)::GetUnit() << "\n";

}