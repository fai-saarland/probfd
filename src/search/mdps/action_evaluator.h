#pragma once

#include "engine_interfaces/action_evaluator.h"
#include "probabilistic_operator.h"

namespace probabilistic {

template<>
class ActionEvaluator<const ProbabilisticOperator*> {
public:
    virtual ~ActionEvaluator() = default;
    value_type::value_t
    operator()(const StateID&, const ProbabilisticOperator* const&) const
    {
        return 0;
    }
};

using ProbabilisticOperatorEvaluator =
    ActionEvaluator<const ProbabilisticOperator*>;

} // namespace probabilistic
