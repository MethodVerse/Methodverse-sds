#include "operator_test.h"

// ---- Create value pairs (N^2) with expected results, otherwise exception ----
// ---- If operation is allowed, a meaningful expected return value pre-calculated is also returned as reference.
// ---- Otherwise, void return type and runtime exception is expected.
template<> inline Sample<INT, INT, INT> makeSample<INT, INT, INT>() { return Sample(2, 3, 5); }
template<> inline Sample<INT, DBL, DBL> makeSample<INT, DBL, DBL>() { return Sample(2, 3.5, 5.5); }
template<> inline Sample<INT, BOO, void> makeSample<INT, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<INT, STR, void> makeSample<INT, STR, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<INT, RV3, RV3> makeSample<INT, RV3, RV3>() { return Sample(2, RV3(1.1,2.2,3.3), RV3(3.1,4.2,5.3)); }
template<> inline Sample<INT, CV3, CV3> makeSample<INT, CV3, CV3>() { return Sample(2, CV3(1.1,2.2,3.3), CV3(3.1,4.2,5.3)); }
template<> inline Sample<INT, MAT3, MAT3> makeSample<INT, MAT3, MAT3>() { return Sample(2, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 3,4,5,6,7,8,9,10,11).finished()); }
template<> inline Sample<INT, QUAD, void> makeSample<INT, QUAD, void>() { throw std::runtime_error("not supported!"); }

template<> inline Sample<DBL, DBL, DBL> makeSample<DBL, DBL, DBL>() { return Sample(2.5, 3.5, 6.0); }
template<> inline Sample<DBL, INT, DBL> makeSample<DBL, INT, DBL>() { return Sample(2.5, 3, 5.5); }
template<> inline Sample<DBL, BOO, void> makeSample<DBL, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<DBL, STR, void> makeSample<DBL, STR, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<DBL, RV3, RV3> makeSample<DBL, RV3, RV3>() { return Sample(2.5, RV3(1.1,2.2,3.3), RV3(3.6,4.7,5.8)); }
template<> inline Sample<DBL, CV3, CV3> makeSample<DBL, CV3, CV3>() { return Sample(2.5, CV3(1.1,2.2,3.3), CV3(3.6,4.7,5.8)); }
template<> inline Sample<DBL, MAT3, MAT3> makeSample<DBL, MAT3, MAT3>() { return Sample(2.5, (MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5).finished()); }
template<> inline Sample<DBL, QUAD, void> makeSample<DBL, QUAD, void>() { throw std::runtime_error("not supported!"); }

template<> inline Sample<BOO, BOO, void> makeSample<BOO, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<BOO, INT, void> makeSample<BOO, INT, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<BOO, DBL, void> makeSample<BOO, DBL, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<BOO, STR, void> makeSample<BOO, STR, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<BOO, RV3, void> makeSample<BOO, RV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<BOO, CV3, void> makeSample<BOO, CV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<BOO, MAT3, void> makeSample<BOO, MAT3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<BOO, QUAD, void> makeSample<BOO, QUAD, void>() { throw std::runtime_error("not supported!"); }

template<> inline Sample<STR, STR, STR> makeSample<STR, STR, STR>() { return Sample(std::string("hello"), std::string("methodverse"), std::string("hellomethodverse")); }
template<> inline Sample<STR, INT, void> makeSample<STR, INT, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<STR, DBL, void> makeSample<STR, DBL, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<STR, BOO, void> makeSample<STR, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<STR, RV3, void> makeSample<STR, RV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<STR, CV3, void> makeSample<STR, CV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<STR, MAT3, void> makeSample<STR, MAT3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<STR, QUAD, void> makeSample<STR, QUAD, void>() { throw std::runtime_error("not supported!"); }

template<> inline Sample<RV3, RV3, RV3> makeSample<RV3, RV3, RV3>() { return Sample(RV3(1,2,3), RV3(4,5,6), RV3(5,7,9)); }
template<> inline Sample<RV3, CV3, void> makeSample<RV3, CV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<RV3, INT, RV3> makeSample<RV3, INT, RV3>() { return Sample(RV3(1,2,3), 2, RV3(3,4,5)); }
template<> inline Sample<RV3, DBL, RV3> makeSample<RV3, DBL, RV3>() { return Sample(RV3(1,2,3), 2.5, RV3(3.5,4.5,5.5)); }
template<> inline Sample<RV3, BOO, void> makeSample<RV3, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<RV3, STR, void> makeSample<RV3, STR, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<RV3, MAT3, void> makeSample<RV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<RV3, QUAD, void> makeSample<RV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

template<> inline Sample<CV3, CV3, CV3> makeSample<CV3, CV3, CV3>() { return Sample(CV3(1,2,3), CV3(4,5,6), CV3(5,7,9)); }
template<> inline Sample<CV3, RV3, void> makeSample<CV3, RV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<CV3, INT, CV3> makeSample<CV3, INT, CV3>() { return Sample(CV3(1,2,3), 2, CV3(3,4,5)); }
template<> inline Sample<CV3, DBL, CV3> makeSample<CV3, DBL, CV3>() { return Sample(CV3(1,2,3), 2.5, CV3(3.5,4.5,5.5)); }
template<> inline Sample<CV3, BOO, void> makeSample<CV3, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<CV3, STR, void> makeSample<CV3, STR, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<CV3, MAT3, void> makeSample<CV3, MAT3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<CV3, QUAD, void> makeSample<CV3, QUAD, void>() { throw std::runtime_error("not supported!"); }

template<> inline Sample<MAT3, MAT3, MAT3> makeSample<MAT3, MAT3, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), (MAT3() << 9,8,7,6,5,4,3,2,1).finished(), (MAT3() << 10,10,10,10,10,10,10,10,10).finished()); }
template<> inline Sample<MAT3, INT, MAT3> makeSample<MAT3, INT, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2, (MAT3() << 3,4,5,6,7,8,9,10,11).finished()); }
template<> inline Sample<MAT3, DBL, MAT3> makeSample<MAT3, DBL, MAT3>() { return Sample((MAT3() << 1,2,3,4,5,6,7,8,9).finished(), 2.5, (MAT3() << 3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5).finished()); }
template<> inline Sample<MAT3, BOO, void> makeSample<MAT3, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<MAT3, STR, void> makeSample<MAT3, STR, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<MAT3, RV3, void> makeSample<MAT3, RV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<MAT3, CV3, void> makeSample<MAT3, CV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<MAT3, QUAD, void> makeSample<MAT3, QUAD, void>() { throw std::runtime_error("not supported!"); }

template<> inline Sample<QUAD, QUAD, QUAD> makeSample<QUAD, QUAD, QUAD>() { return Sample( QUAD(1, 2, 3, 4),  QUAD(4, 3, 2, 1),  QUAD(5, 5, 5, 5));}
template<> inline Sample<QUAD, INT, void> makeSample<QUAD, INT, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<QUAD, DBL, void> makeSample<QUAD, DBL, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<QUAD, BOO, void> makeSample<QUAD, BOO, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<QUAD, STR, void> makeSample<QUAD, STR, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<QUAD, RV3, void> makeSample<QUAD, RV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<QUAD, CV3, void> makeSample<QUAD, CV3, void>() { throw std::runtime_error("not supported!"); }
template<> inline Sample<QUAD, MAT3, void> makeSample<QUAD, MAT3, void>() { throw std::runtime_error("not supported!"); }

// Detects types that support isApprox(other, eps) -> bool
template<class T>
concept HasIsApprox = requires(const T& a, const T& b, double eps) {
    { a.isApprox(b, eps) } -> std::convertible_to<bool>;
};

template<class A, class B>
void ExpectEqualSmart(const A& expected, const B& actual, double eps = 1e-6)
{
    if constexpr (HasIsApprox<A> && HasIsApprox<B>) {
        // Eigen matrices/arrays/quaternions, etc.
        EXPECT_TRUE(expected.isApprox(actual, eps));
    } else if constexpr (std::is_floating_point_v<std::decay_t<A>> &&
                         std::is_floating_point_v<std::decay_t<B>>) {
        EXPECT_NEAR(expected, actual, eps);
    } else {
        // ints, bools, std::string, etc.
        EXPECT_EQ(expected, actual);
    }
}

// ---------- Typed test suite over all pairs ----------
template <class PairT>
class AddOpTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(AddOpTyped, AllPairsT);

TYPED_TEST(AddOpTyped, Add_Operability_And_Correctness_Primitives)
{
    using L = typename TypeParam::left_type;
    using R = typename TypeParam::right_type;

    using CL = category_t<L>;;
    using CR = category_t<R>;

    using Policy = op_policy<CL, CR, add_op>;
    using RT = op_return_t<Policy, L, R>;
    if constexpr (op_allowed<Policy, L, R>) {
        auto pr = makeSample<L, R, RT>();
        auto actual = Policy::impl(pr.left, pr.right);   // test the policy
        auto actual2 = pr.left + pr.right;               // test the operator
        ExpectEqualSmart(pr.result, actual);
        ExpectEqualSmart(pr.result, actual2);
    }
    else {
        static_assert(!op_allowed<Policy, L, R>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<L, R, RT>()), std::runtime_error);
    }
}
