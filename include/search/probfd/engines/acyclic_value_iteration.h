#ifndef PROBFD_ENGINES_ACYCLIC_VALUE_ITERATION_H
#define PROBFD_ENGINES_ACYCLIC_VALUE_ITERATION_H

#include "probfd/engines/engine.h"
#include "probfd/engines/utils.h"

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/policies/map_policy.h"

#include "utils/countdown_timer.h"

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
 * Performs value iteration on acyclic MDPs by performing a depth-first search
 * ending at terminal states and applying Bellman updates during backtracking.
 * Exactly one Bellman update is performed per encountered state.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @remark The search engine does not validate that the state space is acyclic.
 * It is an error to invoke this search engine on state spaces which contain
 * cycles.
 */
template <typename State, typename Action>
class AcyclicValueIteration : public MDPEngine<State, Action> {
    struct StateInfo {
        bool expanded = false;
        std::optional<Action> best_action = std::nullopt;
        value_t value;
    };

    struct IncrementalExpansionInfo {
        const StateID state;

        // Applicable operators left to expand
        std::vector<Action> remaining_aops;

        // The current transition and transition successor
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;

        // The current transition Q-value
        value_t t_value;

        IncrementalExpansionInfo(
            StateID state,
            std::vector<Action> remaining_aops,
            MDPEngine<State, Action>& engine)
            : state(state)
            , remaining_aops(std::move(remaining_aops))
        {
            assert(!this->remaining_aops.empty());
            auto& next_action = remaining_aops.back();
            t_value = engine.get_action_cost(state, next_action);
            transition.clear();
            engine.generate_successors(state, next_action, transition);
            successor = transition.begin();
        }

        bool generate_successor()
        {
            assert(successor != transition.end());
            return ++successor != transition.end();
        }

        bool generate_next_transition(MDPEngine<State, Action>& engine)
        {
            assert(!remaining_aops.empty());
            remaining_aops.pop_back();

            if (remaining_aops.empty()) {
                return false;
            }

            auto& next_action = remaining_aops.back();
            t_value = engine.get_action_cost(state, next_action);
            transition.clear();
            engine.generate_successors(state, next_action, transition);
            successor = transition.begin();

            return true;
        }
    };

    engine_interfaces::Evaluator<State>* prune_;

    Statistics statistics_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::stack<IncrementalExpansionInfo> expansion_stack_;

public:
    AcyclicValueIteration(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* prune = nullptr)
        : MDPEngine<State, Action>(state_space, cost_function)
        , prune_(prune)
    {
    }

    std::unique_ptr<PartialPolicy<State, Action>>
    compute_policy(param_type<State> initial_state, double max_time) override
    {
        std::unique_ptr<policies::MapPolicy<State, Action>> policy(
            new policies::MapPolicy<State, Action>(this->get_state_space()));
        solve(initial_state, max_time, policy.get());
        return policy;
    }

    Interval solve(param_type<State> initial_state, double max_time) override
    {
        return solve(initial_state, max_time, nullptr);
    }

    Interval solve(
        param_type<State> initial_state,
        double max_time,
        policies::MapPolicy<State, Action>* policy)
    {
        utils::CountdownTimer timer(max_time);

        const StateID initial_state_id = this->get_state_id(initial_state);
        StateInfo& iinfo = state_infos_[initial_state_id];

        if (!push_state(initial_state_id, iinfo)) {
            return Interval(iinfo.value);
        }

        do {
            dfs_expand(timer, policy);
        } while (dfs_backtrack(timer, policy));

        assert(expansion_stack_.empty());
        return Interval(iinfo.value);
    }

    void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

private:
    void dfs_expand(
        utils::CountdownTimer& timer,
        policies::MapPolicy<State, Action>* policy)
    {
        IncrementalExpansionInfo* e = &expansion_stack_.top();

        do {
            for (;;) {
                timer.throw_if_expired();

                const auto [succ_id, probability] = *e->successor;
                StateInfo& succ_info = state_infos_[succ_id];

                if (!succ_info.expanded && push_state(succ_id, succ_info)) {
                    e = &expansion_stack_.top();
                    continue; // DFS recursion
                }

                on_backtrack(*e, probability, succ_info);

                if (!e->generate_successor()) {
                    break;
                }
            }

            finalize_transition(*e);
        } while (e->generate_next_transition(*this));

        on_fully_expanded(*e, policy);
    }

    void on_backtrack(
        IncrementalExpansionInfo& e,
        value_t probability,
        StateInfo& succ_info)
    {
        // Update transition Q-value
        e.t_value += probability * succ_info.value;
    }

    void finalize_transition(IncrementalExpansionInfo& e)
    {
        // Minimum Q-value
        StateInfo& info = state_infos_[e.state];

        if (e.t_value < info.value) {
            info.best_action = e.remaining_aops.back();
            info.value = e.t_value;
        }
    }

    void on_fully_expanded(
        IncrementalExpansionInfo& e,
        policies::MapPolicy<State, Action>* policy)
    {
        StateInfo& info = state_infos_[e.state];

        if (policy) {
            policy->emplace_decision(
                e.state,
                *info.best_action,
                Interval(info.value));
        }
    }

    bool dfs_backtrack(
        utils::CountdownTimer& timer,
        policies::MapPolicy<State, Action>* policy)
    {
        IncrementalExpansionInfo* e;

        for (;;) {
            expansion_stack_.pop();

            if (expansion_stack_.empty()) {
                return false;
            }

            timer.throw_if_expired();

            e = &expansion_stack_.top();

            const auto [succ_id, probability] = *e->successor;
            on_backtrack(*e, probability, state_infos_[succ_id]);

            if (e->generate_successor()) {
                return true;
            }

            finalize_transition(*e);

            if (e->generate_next_transition(*this)) {
                return true;
            }

            on_fully_expanded(*e, policy);
        }
    }

    bool push_state(StateID state_id, StateInfo& succ_info)
    {
        assert(!succ_info.expanded);
        succ_info.expanded = true;

        const State state = this->lookup_state(state_id);
        const TerminationInfo term_info = this->get_termination_info(state);
        const value_t value = term_info.get_cost();

        succ_info.value = value;

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
        expansion_stack_.emplace(state_id, std::move(remaining_aops), *this);

        return true;
    }
};

} // namespace acyclic_vi
} // namespace engines
} // namespace probfd

#endif // __ACYCLIC_VALUE_ITERATION_H__