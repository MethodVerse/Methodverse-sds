#include "operator_test.h"

namespace methodverse::operatortest::dot {

// ---------- Typed test suite over all pairs ----------
template <class PairT>
class DotOpTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(DotOpTyped, AllPairsT);

TYPED_TEST(DotOpTyped, Dot_Operability_And_Correctness_Primitives)
{
    using L = typename TypeParam::left_type;
    using R = typename TypeParam::right_type;

    using CL = category_t<L>;;
    using CR = category_t<R>;

    using Policy = op_policy<CL, CR, dot_op>;
    using RT = op_return_t<Policy, L, R>;
    if constexpr (op_allowed<Policy, L, R>) {
        auto pr = makeSample<L, R, RT>();
        auto actual = Policy::impl(pr.left, pr.right);   // test the policy
        auto actual2 = methodverse::parameter::dot(pr.left, pr.right);               // test the operator to here
        ExpectEqualSmart(pr.result, actual);
        ExpectEqualSmart(pr.result, actual2);

        // Test chained addition, N/A

        // ---- Tests on array operations
        // equal size of arrays
        auto larr = std::vector<L>(2, pr.left);
        auto rarr = std::vector<R>(2, pr.right);
        auto arr = methodverse::parameter::dot(larr, rarr);
        ExpectEqualSmart(pr.result, arr[0]);
        ExpectEqualSmart(pr.result, arr[1]);

        // unequal size of arrays, one with single element
        larr = std::vector<L>(1, pr.left);
        rarr = std::vector<R>(2, pr.right);
        arr = methodverse::parameter::dot(larr, rarr);
        ExpectEqualSmart(pr.result, arr[0]);
        ExpectEqualSmart(pr.result, arr[1]);    
        
        // unequal size of arrays, one with single element
        larr = std::vector<L>(2, pr.left);
        rarr = std::vector<R>(1, pr.right);
        arr = methodverse::parameter::dot(larr, rarr);
        ExpectEqualSmart(pr.result, arr[0]);
        ExpectEqualSmart(pr.result, arr[1]);   
        
        // unequal size of arrays, none of them have single element
        larr = std::vector<L>(2, pr.left);
        rarr = std::vector<R>(3, pr.right);
        EXPECT_THROW(arr = methodverse::parameter::dot(larr, rarr), std::runtime_error);
    }
    else {
        static_assert(!op_allowed<Policy, L, R>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<L, R, RT>()), std::runtime_error);
    }
}

}; // namespace