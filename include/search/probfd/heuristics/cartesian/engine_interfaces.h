#ifndef PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/abstraction.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/state_space.h"

#include "probfd/heuristics/cartesian/probabilistic_transition.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"

namespace probfd {
namespace engine_interfaces {

template <>
class StateSpace<
    const heuristics::cartesian::AbstractState*,
    const heuristics::cartesian::ProbabilisticTransition*> {
    heuristics::cartesian::Abstraction& abstraction;

public:
    explicit StateSpace(heuristics::cartesian::Abstraction& abstraction)
        : abstraction(abstraction)
    {
    }

    /**
     * @brief Get the state id for a given state.
     */
    StateID get_state_id(const heuristics::cartesian::AbstractState* state)
    {
        return state->get_id();
    }

    /**
     * @brief Get the state for a given state id.
     */
    const heuristics::cartesian::AbstractState* get_state(StateID state_id)
    {
        return &abstraction.get_state(state_id.id);
    }

    /**
     * @brief Get the action id of a given state action.
     *
     * @param state_id - The ID of the state in which the action is applicable.
     * @param action - The action.
     * @return ActionID - An ID representing this state action.
     */
    ActionID get_action_id(
        StateID,
        const heuristics::cartesian::ProbabilisticTransition* action)
    {
        return ActionID(reinterpret_cast<uintptr_t>(action));
    }

    /**
     * @brief Get the action for a given state and action ID.
     *
     * @param state_id - The ID of the state in which the represented action is
     * applicable.
     * @param action_id - The action ID.
     * @returns const heuristics::cartesian::ProbabilisticTransition* - The
     * represented action.
     */
    const heuristics::cartesian::ProbabilisticTransition*
    get_action(StateID, ActionID action_id)
    {
        return reinterpret_cast<
            const heuristics::cartesian::ProbabilisticTransition*>(
            static_cast<uintptr_t>(action_id.id));
    }

    /**
     * Generates all applicable actions of the state \p state and outputs them
     * in \p result.
     */
    void generate_applicable_actions(
        StateID state,
        std::vector<const heuristics::cartesian::ProbabilisticTransition*>&
            result)
    {
        for (const auto* t : abstraction.get_transition_system()
                                 .get_outgoing_transitions()[state.id]) {
            result.push_back(t);
        }
    }

    void generate_action_transitions(
        StateID,
        const heuristics::cartesian::ProbabilisticTransition* action,
        Distribution<StateID>& result)
    {
        for (size_t i = 0; i != action->target_ids.size(); ++i) {
            const value_t probability =
                abstraction.get_transition_system().get_probability(
                    action->op_id,
                    i);
            result.add_probability(action->target_ids[i], probability);
        }
    }

    void generate_all_transitions(
        StateID state,
        std::vector<const heuristics::cartesian::ProbabilisticTransition*>&
            aops,
        std::vector<Distribution<StateID>>& successors)
    {
        for (const auto* t : abstraction.get_transition_system()
                                 .get_outgoing_transitions()[state.id]) {
            aops.push_back(t);
            generate_action_transitions(state, t, successors.emplace_back());
        }
    }
};

} // namespace engine_interfaces

namespace heuristics {
namespace cartesian {

class CartesianCostFunction
    : public engine_interfaces::
          CostFunction<const AbstractState*, const ProbabilisticTransition*> {
    Abstraction& abstraction;
    std::vector<value_t> operator_costs;

public:
    CartesianCostFunction(
        Abstraction& abstraction,
        std::vector<value_t> operator_costs)
        : abstraction(abstraction)
        , operator_costs(std::move(operator_costs))
    {
    }

    /**
     * @brief Get the termination cost info of the input state.
     */
    TerminationInfo get_termination_info(const AbstractState* const& s) override
    {
        if (abstraction.get_goals().contains(s->get_id())) {
            return TerminationInfo(true, 0.0_vt);
        }

        return TerminationInfo(false, INFINITE_VALUE);
    }

    /**
     * @brief Get the action cost of the action when applied in a state.
     */
    value_t get_action_cost(StateID, const ProbabilisticTransition* t) override
    {
        return operator_costs[t->op_id];
    }
};

class CartesianHeuristic
    : public engine_interfaces::Evaluator<const AbstractState*> {
public:
    EvaluationResult evaluate(const AbstractState* const&) const override
    {
        return EvaluationResult(false, 0.0_vt);
    }

    value_t get_h_value(int) const { abort(); }
    value_t set_h_value(int, int) { abort(); }

    void copy_h_value_to_children(int, int, int) { abort(); }
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif