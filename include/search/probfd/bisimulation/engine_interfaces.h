#ifndef PROBFD_BISIMULATION_ENGINE_INTERFACES_H
#define PROBFD_BISIMULATION_ENGINE_INTERFACES_H

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/types.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/state_space.h"

#include "probfd/interval.h"

namespace probfd {
namespace bisimulation {

using QuotientCostFunction = engine_interfaces::
    CostFunction<bisimulation::QuotientState, bisimulation::QuotientAction>;
using QuotientEvaluator =
    engine_interfaces::Evaluator<bisimulation::QuotientState>;

struct DefaultQuotientEvaluator : public QuotientEvaluator {
    bisimulation::BisimilarStateSpace* bisim_;
    const Interval bound_;
    const value_t default_;

    explicit DefaultQuotientEvaluator(
        bisimulation::BisimilarStateSpace* bisim,
        Interval bound,
        value_t default_value = 0);

    EvaluationResult evaluate(bisimulation::QuotientState state) const override;
};

struct DefaultQuotientCostFunction : public QuotientCostFunction {
    bisimulation::BisimilarStateSpace* bisim_;
    const Interval bound_;
    const value_t default_;

    explicit DefaultQuotientCostFunction(
        bisimulation::BisimilarStateSpace* bisim,
        Interval bound,
        value_t default_value = 0);

    TerminationInfo
    get_termination_info(bisimulation::QuotientState state) override;

    value_t get_action_cost(StateID state, bisimulation::QuotientAction action)
        override;
};

} // namespace bisimulation
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__