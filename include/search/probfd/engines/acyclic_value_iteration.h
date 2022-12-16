#ifndef MDPS_ENGINES_ACYCLIC_VALUE_ITERATION_H
#define MDPS_ENGINES_ACYCLIC_VALUE_ITERATION_H

#include "probfd/engines/engine.h"

#include "probfd/engine_interfaces/state_evaluator.h"

#include "probfd/storage/per_state_storage.h"

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
        value_type::value_t value;
    };

    struct IncrementalExpansionInfo {
        IncrementalExpansionInfo(
            const StateID& state,
            value_type::value_t reward,
            std::vector<Action> remaining_aops)
            : state(state)
            , reward(reward)
            , remaining_aops(std::move(remaining_aops))
        {
            assert(!this->remaining_aops.empty());
        }

        const StateID state;
        const value_type::value_t reward;

        // Applicable operators left to expand
        std::vector<Action> remaining_aops;

        // The current transition and transition successor
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;

        // The current transition Q-value
        value_type::value_t t_value;
    };

public:
    /**
     * @brief Constructs an instance of acyclic value iteration.
     *
     * The parameter \p prune optionally specifies an optional dead-end
     * detector.
     *
     * @see MDPEngine
     */
    explicit AcyclicValueIteration(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* reward_function,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::StateEvaluator<State>* prune = nullptr)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              reward_function,
              transition_generator)
        , prune_(prune)
    {
    }

    virtual value_type::value_t solve(const State& initial_state) override
    {
        if (!push_state(this->get_state_id(initial_state))) {
            return state_infos_[this->get_state_id(initial_state)].value;
        }

        do {
            IncrementalExpansionInfo& e = expansion_stack_.top();

            do {
                // Topological order: Push all successors, recurse if
                // one has not been expanded before
                for (; e.successor != e.transition.end(); ++e.successor) {
                    const auto& succ_id = e.successor->element;
                    if (push_state(succ_id)) {
                        goto continue_outer; // DFS recursion
                    }

                    // Already seen -> update transition Q-value
                    const auto& succ_prob = e.successor->probability;
                    e.t_value += succ_prob * state_infos_[succ_id].value;
                }

                // Maximum Q-value
                StateInfo& info = state_infos_[e.state];
                info.value = std::max(e.t_value, info.value);

                // If no operators are remaining, we are done.
                if (e.remaining_aops.empty()) {
                    break;
                }

                // Otherwise set up the next transition to handle.
                setup_next_transition(e);
            } while (true);

            expansion_stack_.pop();

        continue_outer:;
        } while (!expansion_stack_.empty());

        return state_infos_[this->get_state_id(initial_state)].value;
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

private:
    void setup_next_transition(IncrementalExpansionInfo& e)
    {
        auto& next_action = e.remaining_aops.back();
        e.t_value = e.reward + this->get_action_reward(e.state, next_action);
        e.transition.clear();
        this->generate_successors(e.state, next_action, e.transition);
        e.remaining_aops.pop_back();
        e.successor = e.transition.begin();
    }

    bool push_state(const StateID& state_id)
    {
        if (state_infos_[state_id].expanded) {
            return false;
        }

        const State state = this->lookup_state(state_id);
        const TerminationInfo term_info = this->get_state_reward(state);
        const value_type::value_t value = term_info.get_reward();

        StateInfo& info = state_infos_[state_id];

        info.expanded = true;
        info.value = value;

        if (term_info.is_goal_state()) {
            ++statistics_.terminal_states;
            ++statistics_.goal_states;
            return false;
        }

        if (prune_ && prune_->operator()(state).is_unsolvable()) {
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
        auto& e = expansion_stack_.emplace(
            state_id,
            value,
            std::move(remaining_aops));

        setup_next_transition(e);
        return true;
    }

    engine_interfaces::StateEvaluator<State>* prune_;

    Statistics statistics_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::stack<IncrementalExpansionInfo> expansion_stack_;
};

} // namespace acyclic_vi
} // namespace engines
} // namespace probfd

#endif // __ACYCLIC_VALUE_ITERATION_H__