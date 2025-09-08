#include "operator_test.h"

namespace methodverse::operatortest::mul {
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


// ---------- Typed test suite over all pairs ----------
template <class PairT>
class MulOpTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(MulOpTyped, AllPairsT);

TYPED_TEST(MulOpTyped, Sub_Operability_And_Correctness_Primitives)
{
    using L = typename TypeParam::left_type;
    using R = typename TypeParam::right_type;

    using CL = category_t<L>;;
    using CR = category_t<R>;

    using Policy = op_policy<CL, CR, mul_op>;
    using RT = op_return_t<Policy, L, R>;
    if constexpr (op_allowed<Policy, L, R>) {
        auto pr = makeSample<L, R, RT>();
        auto actual = Policy::impl(pr.left, pr.right);   // test the policy
        auto actual2 = pr.left * pr.right;               // test the operator
        ExpectEqualSmart(pr.result, actual);
        ExpectEqualSmart(pr.result, actual2);
        
        // Test chained multiplication, N/A - not doable for example Matrix * colvector * Matrix.

        // ---- Tests on array operations
        // equal size of arrays
        auto larr = std::vector<L>(2, pr.left);
        auto rarr = std::vector<R>(2, pr.right);
        auto arr = larr * rarr;
        ExpectEqualSmart(pr.result, arr[0]);
        ExpectEqualSmart(pr.result, arr[1]);

        // unequal size of arrays, one with single element
        larr = std::vector<L>(1, pr.left);
        rarr = std::vector<R>(2, pr.right);
        arr = larr * rarr;
        ExpectEqualSmart(pr.result, arr[0]);
        ExpectEqualSmart(pr.result, arr[1]);    
        
        // unequal size of arrays, one with single element
        larr = std::vector<L>(2, pr.left);
        rarr = std::vector<R>(1, pr.right);
        arr = larr * rarr;
        ExpectEqualSmart(pr.result, arr[0]);
        ExpectEqualSmart(pr.result, arr[1]);   
        
        // unequal size of arrays, none of them have single element
        larr = std::vector<L>(2, pr.left);
        rarr = std::vector<R>(3, pr.right);
        EXPECT_THROW(arr = larr * rarr, std::runtime_error); 
    }
    else {
        static_assert(!op_allowed<Policy, L, R>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<L, R, RT>()), std::runtime_error);
    }
}

}; // namespace