#include "action_evaluator.h"

namespace probabilistic {

value_type::value_t
ActionEvaluator<const ProbabilisticOperator*>::operator()(
const StateID& s, const ProbabilisticOperator* const& op) const
{
    return this->evaluate(s, op);
}

}