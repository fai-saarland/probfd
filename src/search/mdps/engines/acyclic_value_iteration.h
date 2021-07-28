#pragma once

#include "../engine_interfaces/state_evaluator.h"
#include "../storage/per_state_storage.h"
#include "engine.h"

#include <iostream>
#include <memory>
#include <stack>

namespace probabilistic {
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
public:
    /**
     * @brief Constructs an instance of acyclic value iteration.
     *
     * @param prune - An optional dead-end detector
     *
     * @see MDPEngine
     */
    explicit AcyclicValueIteration(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        StateEvaluator<State>* prune = nullptr)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              state_reward_function,
              action_reward_function,
              minimal_reward,
              maximal_reward,
              aops_generator,
              transition_generator)
        , prune_(prune)
        , expanded_states_(false)
        , values_(this->get_minimal_reward())
        , expansion_stack_()
    {
    }

    virtual void solve(const State& initial_state) override
    {
        push_state<false>(this->get_state_id(initial_state), nullptr);
        while (!expansion_stack_.empty()) {
            IncrementalExpansionInfo& e = expansion_stack_.top();

            do {
                // Topological order: Push all successors, recurse on stack if
                // one has not been expanded before
                for (; e.successor != e.transition.end(); ++e.successor) {
                    if (push_state(e.successor->first, &e)) {
                        goto break_outer; // break nested loop
                    }
                }

                // Maximum Q-value
                if (e.t_value > e.value) {
                    e.value = e.t_value;
                }

                // If no operators are remaining, we are done. Otherwise set up
                // the next transition to handle.
                if (e.remaining_aops.empty()) {
                    values_[e.state] = e.value;
                    expansion_stack_.pop();
                    break;
                }

                setup_next(e);
            } while (true);
        break_outer:;
        }
    }

    virtual value_type::value_t get_result(const State& initial_state) override
    {
        return values_[this->get_state_id(initial_state)];
    }

    virtual void print_statistics(std::ostream& out) const
    {
        statistics_.print(out);
    }

private:
    struct IncrementalExpansionInfo {
        IncrementalExpansionInfo(
            const StateID& state,
            value_type::value_t reward,
            value_type::value_t value,
            std::vector<Action>&& remaining_aops)
            : state(state)
            , reward(reward)
            , value(value)
            , remaining_aops(std::move(remaining_aops))
        {
            assert(!remaining_aops.empty());
        }
        IncrementalExpansionInfo(IncrementalExpansionInfo&& other) = default;

        const StateID state;
        const value_type::value_t reward;
        value_type::value_t value;

        // Appplicable operators left to expand
        std::vector<Action> remaining_aops;

        // The current expansion
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;
        value_type::value_t t_value;
    };

    void setup_next(IncrementalExpansionInfo& e)
    {
        e.t_value = e.reward +
                    this->get_action_reward(e.state, e.remaining_aops.back());
        e.transition.clear();
        this->generate_successors(
            e.state,
            e.remaining_aops.back(),
            e.transition);
        e.remaining_aops.pop_back();
        e.successor = e.transition.begin();
    }

    template <bool Update = true>
    bool push_state(const StateID& state_ref, IncrementalExpansionInfo* in)
    {
        (void)in;

        if (expanded_states_[state_ref]) {
            if constexpr (Update) {
                in->t_value += in->successor->second * values_[state_ref];
            }
            return false;
        }
        State state = this->lookup_state(state_ref);
        expanded_states_[state_ref] = true;
        if (prune_ && bool(prune_->operator()(state))) {
            ++statistics_.pruned;
            if constexpr (Update) {
                in->t_value +=
                    in->successor->second * this->get_minimal_reward();
            }
            return false;
        }
        auto rew = this->get_state_reward(state);
        if (bool(rew)) {
            ++statistics_.terminal_states;
            ++statistics_.goal_states;
            values_[state_ref] = value_type::value_t(rew);
            if constexpr (Update) {
                in->t_value += in->successor->second * value_type::value_t(rew);
            }
            return false;
        }
        std::vector<Action> remaining_aops;
        this->generate_applicable_ops(state_ref, remaining_aops);
        if (remaining_aops.empty()) {
            ++statistics_.terminal_states;
            if constexpr (Update) {
                in->t_value +=
                    in->successor->second * this->get_minimal_reward();
            }
            return false;
        }
        ++statistics_.state_expansions;
        auto& e = expansion_stack_.emplace(
            state_ref,
            value_type::value_t(rew),
            this->get_minimal_reward(),
            std::move(remaining_aops));

        setup_next(e);
        return true;
    }

    StateEvaluator<State>* prune_;

    Statistics statistics_;

    storage::PerStateStorage<bool> expanded_states_;
    storage::PerStateStorage<value_type::value_t> values_;
    std::stack<IncrementalExpansionInfo> expansion_stack_;
};

} // namespace acyclic_vi
} // namespace engines
} // namespace probabilistic
