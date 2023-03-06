#ifndef PROBFD_ENGINES_ACYCLIC_VALUE_ITERATION_H
#define PROBFD_ENGINES_ACYCLIC_VALUE_ITERATION_H

#include "probfd/engines/engine.h"
#include "probfd/engines/utils.h"

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/policies/map_policy.h"

#include <iostream>
#include <memory>
#include <stack>

namespace probfd {
namespace engines {

/// Namespace dedicated to the acyclic value iteration algorithm.
namespace acyclic_vi {

/**
 * Acyclic value iteration statistics.
 */
struct Statistics {
    unsigned long long state_expansions = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long pruned = 0;

    void print(std::ostream& out) const
    {
        out << "  Expanded state(s): " << state_expansions << std::endl;
        out << "  Pruned state(s): " << pruned << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
    }
};

/**
 * @brief Implements acyclic Value Iteration.
 *
 * Performs value iteration on acyclic MDPs using an optimal (topological)
 * update order. Each state only needs to be updated once.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @remark Does not validate that the input model is acyclic.
 */
template <typename State, typename Action>
class AcyclicValueIteration : public MDPEngine<State, Action> {
    struct StateInfo {
        bool expanded = false;
        ActionID best_action = ActionID::undefined;
        value_t value;
    };

    struct IncrementalExpansionInfo {
        IncrementalExpansionInfo(
            StateID state,
            std::vector<Action> remaining_aops)
            : state(state)
            , remaining_aops(std::move(remaining_aops))
        {
            assert(!this->remaining_aops.empty());
        }

        const StateID state;

        // Applicable operators left to expand
        std::vector<Action> remaining_aops;

        // The current transition and transition successor
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;

        // The current transition Q-value
        value_t t_value;
    };

    engine_interfaces::Evaluator<State>* prune_;

    Statistics statistics_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::stack<IncrementalExpansionInfo> expansion_stack_;

public:
    /**
     * @brief Constructs an instance of acyclic value iteration.
     *
     * The parameter \p prune optionally specifies an optional dead-end
     * detector.
     *
     * @see MDPEngine
     */
    AcyclicValueIteration(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* prune = nullptr)
        : MDPEngine<State, Action>(state_space, cost_function)
        , prune_(prune)
    {
    }

    std::unique_ptr<PartialPolicy<State, Action>>
    compute_policy(const State& initial_state) override
    {
        std::unique_ptr<policies::MapPolicy<State, Action>> policy(
            new policies::MapPolicy<State, Action>(this->get_state_space()));
        solve(initial_state, policy.get());
        return policy;
    }

    Interval solve(const State& initial_state) override
    {
        return solve(initial_state, nullptr);
    }

    Interval solve(
        const State& initial_state,
        policies::MapPolicy<State, Action>* policy)
    {
        const StateID initial_state_id = this->get_state_id(initial_state);
        if (!push_state(initial_state_id)) {
            return Interval(state_infos_[initial_state_id].value);
        }

        do {
            IncrementalExpansionInfo& e = expansion_stack_.top();

            for (;;) {
                // Topological order: Push all successors, recurse if
                // one has not been expanded before
                for (; e.successor != e.transition.end(); ++e.successor) {
                    const auto [succ_id, probability] = *e.successor;
                    if (push_state(succ_id)) {
                        goto continue_outer; // DFS recursion
                    }

                    // Already seen -> update transition Q-value
                    e.t_value += probability * state_infos_[succ_id].value;
                }

                // Minimum Q-value
                StateInfo& info = state_infos_[e.state];

                if (e.t_value < info.value) {
                    info.best_action = this->get_action_id(
                        e.state,
                        e.remaining_aops.back());
                    info.value = e.t_value;
                }

                assert(!e.remaining_aops.empty());
                e.remaining_aops.pop_back();

                // If no operators are remaining, we are done.
                if (e.remaining_aops.empty()) {
                    if (policy) {
                        policy->emplace_decision(
                            e.state,
                            info.best_action,
                            Interval(info.value));
                    }

                    break;
                }

                // Otherwise set up the next transition to handle.
                setup_next_transition(e);
            }

            expansion_stack_.pop();

        continue_outer:;
        } while (!expansion_stack_.empty());

        return Interval(state_infos_[initial_state_id].value);
    }

    void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

private:
    void setup_next_transition(IncrementalExpansionInfo& e)
    {
        auto& next_action = e.remaining_aops.back();
        e.t_value = this->get_action_cost(e.state, next_action);
        e.transition.clear();
        this->generate_successors(e.state, next_action, e.transition);
        e.successor = e.transition.begin();
    }

    bool push_state(StateID state_id)
    {
        StateInfo& info = state_infos_[state_id];

        if (info.expanded) {
            return false;
        }

        const State state = this->lookup_state(state_id);
        const TerminationInfo term_info = this->get_termination_info(state);
        const value_t value = term_info.get_cost();

        info.expanded = true;
        info.value = value;

        if (term_info.is_goal_state()) {
            ++statistics_.terminal_states;
            ++statistics_.goal_states;
            return false;
        }

        if (prune_ && prune_->evaluate(state).is_unsolvable()) {
            ++statistics_.pruned;
            return false;
        }

        std::vector<Action> remaining_aops;
        this->generate_applicable_ops(state_id, remaining_aops);
        if (remaining_aops.empty()) {
            ++statistics_.terminal_states;
            return false;
        }

        ++statistics_.state_expansions;
        auto& e = expansion_stack_.emplace(state_id, std::move(remaining_aops));

        setup_next_transition(e);
        return true;
    }
};

} // namespace acyclic_vi
} // namespace engines
} // namespace probfd

#endif // __ACYCLIC_VALUE_ITERATION_H__