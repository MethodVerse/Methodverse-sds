#include "operator_test.h"

namespace methodverse::operatortest::transpose {

// ---------- Typed test suite over all pairs ----------
template <class PrimitiveT>
class TransposeOpTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(TransposeOpTyped, AllTypesT);

TYPED_TEST(TransposeOpTyped, Transpose_Operability_And_Correctness_Primitives)
{
    using T = TypeParam;
    using CT = category_t<T>;

    using Policy = op_policy<CT, void, transpose_op>;
    using RT = op_return_t<Policy, T>;
    if constexpr (op_allowed<Policy, T>) {
        auto pr = makeSample<T, RT>();
        auto actual = Policy::impl(pr.left);   // test the policy
        auto actual2 = methodverse::parameter::transpose(pr.left);               // test the operator to here
        ExpectEqualSmart(pr.result, actual);
        ExpectEqualSmart(pr.result, actual2);

        // Test chained addition, N/A

        // ---- Tests on array operations
        // equal size of arrays
        auto larr = std::vector<T>(2, pr.left);
        auto arr = methodverse::parameter::transpose(larr);
        ExpectEqualSmart(pr.result, arr[0]);
        ExpectEqualSmart(pr.result, arr[1]);
    }
    else {
        static_assert(!op_allowed<Policy, T>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<T, RT>()), std::runtime_error);
    }
}

}; // namespace