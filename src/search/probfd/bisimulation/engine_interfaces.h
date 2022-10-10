#ifndef MDPS_BISIMULATION_ENGINE_INTERFACES_H
#define MDPS_BISIMULATION_ENGINE_INTERFACES_H

#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/reward_function.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/transition_generator.h"

#include "bisimilar_state_space.h"

#include "../value_utils.h"

namespace probfd {

template <>
struct StateIDMap<bisimulation::QuotientState> {
    StateID get_state_id(const bisimulation::QuotientState& state) const;
    bisimulation::QuotientState get_state(const StateID& state_id) const;
};

template <>
struct ActionIDMap<bisimulation::QuotientAction> {
    ActionID get_action_id(
        const StateID& state_id,
        const bisimulation::QuotientAction& action) const;
    bisimulation::QuotientAction
    get_action(const StateID& state_id, const ActionID& action) const;
};

template <>
struct TransitionGenerator<bisimulation::QuotientAction> {
    explicit TransitionGenerator(bisimulation::BisimilarStateSpace* bisim);
    void operator()(
        const StateID& state,
        std::vector<bisimulation::QuotientAction>& result) const;
    void operator()(
        const StateID& state,
        const bisimulation::QuotientAction& action,
        Distribution<StateID>& result) const;
    void operator()(
        const StateID& state,
        std::vector<bisimulation::QuotientAction>& aops,
        std::vector<Distribution<StateID>>& result) const;
    bisimulation::BisimilarStateSpace* bisim_;
};

namespace bisimulation {

using QuotientRewardFunction =
    RewardFunction<bisimulation::QuotientState, bisimulation::QuotientAction>;
using QuotientStateEvaluator = StateEvaluator<bisimulation::QuotientState>;

struct DefaultQuotientStateEvaluator : public QuotientStateEvaluator {
    explicit DefaultQuotientStateEvaluator(
        bisimulation::BisimilarStateSpace* bisim,
        const value_utils::IntervalValue bound,
        value_type::value_t default_value = 0);

    EvaluationResult
    evaluate(const bisimulation::QuotientState& state) const override;

    bisimulation::BisimilarStateSpace* bisim_;
    const value_utils::IntervalValue bound_;
    const value_type::value_t default_;
};

struct DefaultQuotientRewardFunction : public QuotientRewardFunction {
    explicit DefaultQuotientRewardFunction(
        bisimulation::BisimilarStateSpace* bisim,
        const value_utils::IntervalValue bound,
        value_type::value_t default_value = 0);

    EvaluationResult
    evaluate(const bisimulation::QuotientState& state) override;

    value_type::value_t
    evaluate(StateID state, bisimulation::QuotientAction action) override;

    bisimulation::BisimilarStateSpace* bisim_;
    const value_utils::IntervalValue bound_;
    const value_type::value_t default_;
};

} // namespace bisimulation
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__