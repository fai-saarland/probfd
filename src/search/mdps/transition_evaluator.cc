#include "transition_evaluator.h"

namespace probabilistic {
namespace algorithms {

value_type::value_t
TransitionRewardFunction<GlobalState, const ProbabilisticOperator*>::operator()(
    const GlobalState& state,
    const ProbabilisticOperator* const& op)
{
    return this->evaluate(state, op);
}

value_type::value_t
TransitionRewardFunction<GlobalState, const ProbabilisticOperator*>::evaluate(
    const GlobalState&,
    const ProbabilisticOperator*)
{
    return value_type::zero;
}

} // namespace algorithms
#if 0
value_type::value_t
TransitionEvaluator::operator()(
    const GlobalState&,
    const std::pair<GlobalState, const ProbabilisticOperator*>& op)
{
    return this->operator()(op.first, op.second);
}
#endif

} // namespace probabilistic
