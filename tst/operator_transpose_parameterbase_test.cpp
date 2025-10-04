#include "operator_test.h"
#include <mp-units/systems/si.h>
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <methodverse/parameter/parameterbase.h>
#include <methodverse/parameter/parameterbase_operator.h>

namespace methodverse::operatortest::transpose {

// ---------- Typed test suite over all pairs ----------
template <class PrimitiveT>
class TransposeOpParameterTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(TransposeOpParameterTyped, AllTypesT);

TYPED_TEST(TransposeOpParameterTyped, Transpose_Operability_And_Correctness_Primitives)
{
    using T = TypeParam;
    using CT = category_t<T>;

    using Policy = op_policy<CT, void, transpose_op>;
    using RT = op_return_t<Policy, T>;
    if constexpr (op_allowed<Policy, T>) {
        auto pr = makeSample<T, RT>();

        // ---- Tests on array operations
        ParameterBase<T, si::tesla> p1({pr.left, pr.left});
        auto par = methodverse::parameter::transpose(p1);
        ExpectEqualSmart(pr.result, par[0]);
        ExpectEqualSmart(pr.result, par[1]);
    }
    else {
        static_assert(!op_allowed<Policy, T>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<T, RT>()), std::runtime_error);
    }
}

}; // namespace