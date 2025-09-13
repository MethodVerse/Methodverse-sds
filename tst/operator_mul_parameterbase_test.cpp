#include "operator_test.h"
#include <mp-units/systems/si.h>
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <methodverse/parameter/parameterbase.h>
#include <methodverse/parameter/parameterbase_operator.h>

using namespace methodverse::parameter;

namespace methodverse::operatortest::mul {

// ---------- Typed test suite over all pairs ----------
template <class PairT>
class MulOpParameterTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(MulOpParameterTyped, AllPairsT);

TYPED_TEST(MulOpParameterTyped, Mul_Operability_And_Correctness_Primitives)
{
    using L = typename TypeParam::left_type;
    using R = typename TypeParam::right_type;

    using CL = category_t<L>;;
    using CR = category_t<R>;

    using Policy = op_policy<CL, CR, mul_op>;
    using RT = op_return_t<Policy, L, R>;
    if constexpr (op_allowed<Policy, L, R>) {
        auto pr = makeSample<L, R, RT>();
        //auto actual = Policy::impl(pr.left, pr.right);   // test the policy
        auto actual = pr.left * pr.right;               // test the operator

        ParameterBase<L, si::tesla / si::metre> p1({pr.left, pr.left});
        ParameterBase<R, si::second> p2({pr.right, pr.right});   
        auto par = p1 * p2; 
        
        ExpectEqualSmart(actual, par[0]);
        ExpectEqualSmart(actual, par[1]);

        static_assert(si::tesla / si::metre * si::second == decltype(par)::GetUnit());
        
    }
    else {
        static_assert(!op_allowed<Policy, L, R>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<L, R, RT>()), std::runtime_error);
    }
}

}; // namespace