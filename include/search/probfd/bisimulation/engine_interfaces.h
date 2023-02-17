#ifndef MDPS_BISIMULATION_ENGINE_INTERFACES_H
#define MDPS_BISIMULATION_ENGINE_INTERFACES_H

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/types.h"

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/state_id_map.h"
#include "probfd/engine_interfaces/transition_generator.h"

#include "probfd/value_utils.h"

namespace probfd {
namespace engine_interfaces {

template <>
struct StateIDMap<bisimulation::QuotientState> {
    StateID get_state_id(const bisimulation::QuotientState& state) const;
    bisimulation::QuotientState get_state(StateID state_id) const;
};

template <>
struct ActionIDMap<bisimulation::QuotientAction> {
    ActionID
    get_action_id(StateID state_id, const bisimulation::QuotientAction& action)
        const;

    bisimulation::QuotientAction
    get_action(StateID state_id, ActionID action) const;
};

template <>
struct TransitionGenerator<bisimulation::QuotientAction> {
    bisimulation::BisimilarStateSpace* bisim_;

    explicit TransitionGenerator(bisimulation::BisimilarStateSpace* bisim);

    void generate_applicable_actions(
        StateID state,
        std::vector<bisimulation::QuotientAction>& result) const;
    void generate_action_transitions(
        StateID state,
        const bisimulation::QuotientAction& action,
        Distribution<StateID>& result) const;
    void generate_all_transitions(
        StateID state,
        std::vector<bisimulation::QuotientAction>& aops,
        std::vector<Distribution<StateID>>& result) const;
};
} // namespace engine_interfaces

namespace bisimulation {

using QuotientCostFunction = engine_interfaces::
    CostFunction<bisimulation::QuotientState, bisimulation::QuotientAction>;
using QuotientStateEvaluator =
    engine_interfaces::StateEvaluator<bisimulation::QuotientState>;

struct DefaultQuotientStateEvaluator : public QuotientStateEvaluator {
    bisimulation::BisimilarStateSpace* bisim_;
    const Interval bound_;
    const value_t default_;

    explicit DefaultQuotientStateEvaluator(
        bisimulation::BisimilarStateSpace* bisim,
        Interval bound,
        value_t default_value = 0);

    EvaluationResult
    evaluate(const bisimulation::QuotientState& state) const override;
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
    get_termination_info(const bisimulation::QuotientState& state) override;

    value_t get_action_cost(StateID state, bisimulation::QuotientAction action)
        override;
};

} // namespace bisimulation
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__