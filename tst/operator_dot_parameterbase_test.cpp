#include "operator_test.h"
#include <mp-units/systems/si.h>
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <methodverse/parameter/parameterbase.h>
#include <methodverse/parameter/parameterbase_operator.h>

namespace methodverse::operatortest::dot {

// ---------- Typed test suite over all pairs ----------
template <class PairT>
class DotOpParameterTyped : public ::testing::Test{
};

TYPED_TEST_SUITE(DotOpParameterTyped, AllPairsT);

TYPED_TEST(DotOpParameterTyped, Dot_Operability_And_Correctness_Primitives)
{
    using L = typename TypeParam::left_type;
    using R = typename TypeParam::right_type;

    using CL = category_t<L>;;
    using CR = category_t<R>;

    using Policy = op_policy<CL, CR, dot_op>;
    using RT = op_return_t<Policy, L, R>;
    if constexpr (op_allowed<Policy, L, R>) {
        auto pr = makeSample<L, R, RT>();
        auto actual = methodverse::parameter::dot(pr.left, pr.right);               // test the operator to here

        // ---- Tests on array operations
        // equal size of arrays
        ParameterBase<L, si::tesla> p1({pr.left, pr.left});
        ParameterBase<R, si::metre> p2({pr.right, pr.right});   
        auto par = methodverse::parameter::dot(p1, p2); 
        ExpectEqualSmart(actual, par[0]);
        ExpectEqualSmart(actual, par[1]);

        // unequal size of arrays, one with single element
        p1.Set({pr.left});
        p2.Set({pr.right, pr.right});
        par = methodverse::parameter::dot(p1, p2);
        ExpectEqualSmart(actual, par[0]);
        ExpectEqualSmart(actual, par[1]);   
        
        // unequal size of arrays, one with single element
        p1.Set({pr.left, pr.left});
        p2.Set({pr.right});
        par = methodverse::parameter::dot(p1, p2);
        ExpectEqualSmart(actual, par[0]);
        ExpectEqualSmart(actual, par[1]);   
        
        // unequal size of arrays, none of them have single element
        p1.Set({pr.left, pr.left});
        p2.Set({pr.right, pr.right, pr.right});
        EXPECT_THROW(par = methodverse::parameter::dot(p1, p2), std::runtime_error);

        //static_assert(si::tesla * si::metre == decltype(par)::GetUnit());
    }
    else {
        static_assert(!op_allowed<Policy, L, R>);
        static_assert(std::is_same_v<RT, void>);
        EXPECT_THROW((makeSample<L, R, RT>()), std::runtime_error);
    }
}

}; // namespace