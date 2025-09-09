#include "operator_test.h"
#include <mp-units/systems/si.h>
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <methodverse/parameter/parameterbase.h>
#include <methodverse/parameter/parameterbase_operator.h>

using namespace methodverse::parameter;

namespace methodverse::operatortest::parameterbase_div {
    // Helpper to convert a value to type 
   // Define the types and units to be tested, and the resulting Param<T,U> types
    using any_unit = std::variant<
        decltype(si::metre),
        decltype(si::second),
        decltype(si::tesla),
        decltype(si::kilogram),
        decltype(si::ampere)
    >;
    
    any_unit random_unit() {
        static std::mt19937 gen(std::random_device{}());
        static std::uniform_int_distribution<int> dist(0, 4); // 4 units
        int idx = dist(gen);

        switch (idx) {
            case 0: return si::metre;
            case 1: return si::second;
            case 2: return si::tesla;
            case 3: return si::kilogram;
            case 4: return si::ampere;
        }
        return si::metre; // fallback
    }

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

// ---------- Typed test suite over all pairs ----------
template <class PairT>
class DivOpParameterTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(DivOpParameterTyped, AllPairsT);

TYPED_TEST(DivOpParameterTyped, Div_Operability_And_Correctness_Primitives)
{
    using L = typename TypeParam::left_type;
    using R = typename TypeParam::right_type;

    using CL = category_t<L>;;
    using CR = category_t<R>;

    using Policy = op_policy<CL, CR, div_op>;
    using RT = op_return_t<Policy, L, R>;
    if constexpr (op_allowed<Policy, L, R>) {
        auto pr = makeSample<L, R, RT>();
        //auto actual = Policy::impl(pr.left, pr.right);   // test the policy
        auto actual = pr.left / pr.right;               // test the operator

        ParameterBase<L, si::tesla> p1({pr.left, pr.left});
        ParameterBase<R, si::metre> p2({pr.right, pr.right});   
        auto par = p1 / p2; 
        
        ExpectEqualSmart(actual, par[0]);
        ExpectEqualSmart(actual, par[1]);

        static_assert(si::tesla / si::metre == decltype(par)::GetUnit());
        
    }
    else {
        static_assert(!op_allowed<Policy, L, R>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<L, R, RT>()), std::runtime_error);
    }
}

}; // namespace