#ifndef PROBFD_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H
#define PROBFD_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H

#include "probfd/engines/engine.h"
#include "probfd/engines/utils.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/policies/map_policy.h"

#include "utils/iterators.h"

#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

/// Namespace dedicated to Topological Value Iteration (TVI).
namespace topological_vi {

/**
 * @brief Topological value iteration statistics.
 */
struct Statistics {
    unsigned long long expanded_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long sccs = 0;
    unsigned long long singleton_sccs = 0;
    unsigned long long bellman_backups = 0;
    unsigned long long pruned = 0;

    void print(std::ostream& out) const
    {
        out << "  Expanded state(s): " << expanded_states << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
        out << "  Pruned state(s): " << pruned << std::endl;
        out << "  Maximal SCCs: " << sccs << " (" << singleton_sccs
            << " are singleton)" << std::endl;
        out << "  Bellman backups: " << bellman_backups << std::endl;
    }
};

/**
 * @brief Implements Topological Value Iteration \cite dai:etal:jair-11.
 *
 * This algorithm performs value iteration on each strongly connected component
 * of the MDP in reverse topological order. This implementation exhaustively
 * explores the entire reachable state space and computes a full optimal state
 * value table by default. A heuristic can be supplied to explicitly prune
 * parts of the state space or to accelerate convergence.
 *
 * This implementation also supports UseInterval bounds. However, convergence
 * is not guaranteed for UseInterval bounds if traps exist within the reachable
 * state space. In this case, traps must be removed prior to running topological
 * value iteration.
 *
 * @see interval_iteration::IntervalIteration
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam UseInterval - Whether bounded value iteration is used.
 */
template <typename State, typename Action, bool UseInterval = false>
class TopologicalValueIteration : public MDPEngine<State, Action> {
    using EngineValueType = engines::EngineValueType<UseInterval>;

    struct StateInfo {
        // Status Flags
        enum { NEW, ONSTACK, CLOSED };

        unsigned stack_id = 0;
        uint8_t status = NEW;
    };

    struct ExplorationInfo {
        // Immutable info
        StateID state_id;  // State this information belongs to
        unsigned stackidx; // Index on the stack of the associated state
        unsigned lowlink;

        // Mutable info
        std::vector<Action> aops;         // Remaining unexpanded operators
        Distribution<StateID> transition; // Currently expanded transition
        // Next state to expand
        typename Distribution<StateID>::const_iterator successor;

        // Probability to remain in the same state.
        value_t self_loop_prob = 0_vt;

        ExplorationInfo(
            StateID state_id,
            unsigned stackidx,
            std::vector<Action> aops,
            Distribution<StateID> transition)
            : state_id(state_id)
            , stackidx(stackidx)
            , lowlink(stackidx)
            , aops(std::move(aops))
            , transition(std::move(transition))
            , successor(this->transition.begin())
        {
        }

        void update_lowlink(unsigned upd) { lowlink = std::min(lowlink, upd); }

        /**
         * Advances to the next non-loop action. Returns nullptr if such an
         * action does not exist.
         */
        bool next_action(TopologicalValueIteration* self, unsigned int state_id)
        {
            for (aops.pop_back(); !aops.empty(); aops.pop_back()) {
                transition.clear();
                self->generate_successors(state_id, aops.back(), transition);

                if (!transition.is_dirac(state_id)) {
                    successor = transition.begin();
                    self_loop_prob = 0_vt;
                    return true;
                }
            }

            return false;
        }

        bool next_successor() { return ++successor != transition.end(); }

        Action& get_current_action() { return aops.back(); }

        ItemProbabilityPair<StateID> get_current_successor()
        {
            return *successor;
        }
    };

    struct QValueInfo {
        // The action id this Q value belongs to.
        ActionID action_id;

        // Precomputed part of the Q-value.
        // Sum of action cost plus those weighted successor values which
        // have already converged due to topological ordering.
        EngineValueType conv_part;

        // Pointers to successor values which have not yet converged,
        // self-loops excluded.
        std::vector<ItemProbabilityPair<EngineValueType*>> nconv_successors;

        QValueInfo(ActionID action_id, value_t action_cost)
            : action_id(action_id)
            , conv_part(action_cost)
        {
        }

        bool has_no_scc_successors() { return nconv_successors.empty(); }

        void normalize(value_t self_loop_prob)
        {
            assert(!nconv_successors.empty());
            assert(0_vt <= self_loop_prob && self_loop_prob < 1_vt);

            if (self_loop_prob != 0_vt) {
                const auto normalization = 1 / (1_vt - self_loop_prob);

                for (auto& [_, prob] : nconv_successors) {
                    prob *= normalization;
                }

                conv_part *= normalization;
            }
        }

        EngineValueType compute_q_value() const
        {
            EngineValueType res = conv_part;

            for (auto& [value, prob] : nconv_successors) {
                res += prob * (*value);
            }

            return res;
        }
    };

    struct StackInfo {
        StateID state_id;

        // Reference to the state value of the state.
        EngineValueType* value;

        // Precomputed part of the max of the value update.
        // Minimum over all Q values of actions leaving the SCC.
        EngineValueType conv_part;

        // Remaining Q values which have not yet converged.
        std::vector<QValueInfo> nconv_qs;

        // ID of the optimal action
        ActionID best_action_id = ActionID::undefined;

        // ID of the optimal action among those leaving the SCC.
        ActionID best_converged_id = ActionID::undefined;

        StackInfo(
            StateID state_id,
            EngineValueType& value_ref,
            value_t terminal_cost,
            unsigned num_aops)
            : state_id(state_id)
            , value(&value_ref)
            , conv_part(terminal_cost)
        {
            nconv_qs.reserve(num_aops);
        }

        bool update_value()
        {
            EngineValueType v = conv_part;
            best_action_id = best_converged_id;

            for (const QValueInfo& info : nconv_qs) {
                if (set_min(v, info.compute_q_value())) {
                    best_action_id = info.action_id;
                }
            }

            if constexpr (UseInterval) {
                return update(*value, v) ||
                       !value->bounds_approximately_equal();
            } else {
                return update(*value, v);
            }
        }
    };

    const engine_interfaces::Evaluator<State>* value_initializer_;
    const bool expand_goals_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    Statistics statistics_;

public:
    TopologicalValueIteration(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        const engine_interfaces::Evaluator<State>* value_initializer,
        bool expand_goals)
        : MDPEngine<State, Action>(state_space, cost_function)
        , value_initializer_(value_initializer)
        , expand_goals_(expand_goals)
    {
    }

    /**
     * \copydoc MDPEngine::compute_policy(const State&)
     */
    std::unique_ptr<PartialPolicy<State, Action>>
    compute_policy(const State& state) override
    {
        storage::PerStateStorage<EngineValueType> value_store;
        std::unique_ptr<policies::MapPolicy<State, Action>> policy(
            new policies::MapPolicy<State, Action>(this->get_state_space()));
        this->solve(this->get_state_id(state), value_store, policy.get());
        return policy;
    }

    /**
     * \copydoc MDPEngine::solve(const State&)
     */
    Interval solve(const State& state) override
    {
        storage::PerStateStorage<EngineValueType> value_store;
        return this->solve(this->get_state_id(state), value_store);
    }

    /**
     * \copydoc MDPEngineInterface::print_statistics(std::ostream&) const
     */
    void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    /**
     * @brief Retreive the algorithm statistics.
     */
    Statistics get_statistics() const { return statistics_; }

    /**
     * @brief Runs the algorithm with the supplied state value storage.
     *
     * Computes the full optimal value function for the entire state space
     * reachable from \p initial_state. Stores the state values in the
     * output parameter \p value_store. Returns the value of the initial state.
     */
    template <typename ValueStore>
    Interval solve(
        StateID init_state_id,
        ValueStore& value_store,
        policies::MapPolicy<State, Action>* policy = nullptr)
    {
        StateInfo& iinfo = state_information_[init_state_id];
        EngineValueType& init_value = value_store[init_state_id];

        if (!push_state(init_state_id, iinfo, init_value)) {
            if constexpr (UseInterval) {
                return init_value;
            } else {
                return Interval(init_value, INFINITE_VALUE);
            }
        }

        ExplorationInfo* explore = &exploration_stack_.back();
        StateID state_id = explore->state_id;
        StackInfo* stack_info = &stack_[explore->stackidx];

        for (;;) {
            while (successor_loop(
                *explore,
                *stack_info,
                state_id,
                value_store)) {
                explore = &exploration_stack_.back();
                state_id = explore->state_id;
                stack_info = &stack_[explore->stackidx];
            }

            for (;;) {
                // Check if an SCC was found.
                const unsigned stack_id = explore->stackidx;
                const unsigned lowlink = explore->lowlink;
                const bool onstack = stack_id != lowlink;

                if (!onstack) {
                    const auto begin = stack_.begin() + stack_id;
                    const auto end = stack_.end();
                    scc_found(begin, end, policy);
                }

                exploration_stack_.pop_back();

                if (exploration_stack_.empty()) {
                    if constexpr (UseInterval) {
                        return init_value;
                    } else {
                        return Interval(init_value, INFINITE_VALUE);
                    }
                }

                explore = &exploration_stack_.back();
                state_id = explore->state_id;
                stack_info = &stack_[explore->stackidx];

                const auto [succ_id, prob] = explore->get_current_successor();
                EngineValueType& s_value = value_store[succ_id];
                QValueInfo& tinfo = stack_info->nconv_qs.back();

                if (onstack) {
                    explore->update_lowlink(lowlink);
                    tinfo.nconv_successors.emplace_back(&s_value, prob);
                } else {
                    tinfo.conv_part += prob * s_value;
                }

                if (explore->next_successor()) {
                    break;
                }

                if (tinfo.has_no_scc_successors()) {
                    if (set_min(stack_info->conv_part, tinfo.conv_part)) {
                        stack_info->best_converged_id = tinfo.action_id;
                    }
                    stack_info->nconv_qs.pop_back();
                } else {
                    tinfo.normalize(explore->self_loop_prob);
                }

                if (explore->next_action(this, state_id)) {
                    Action& action = explore->get_current_action();
                    stack_info->nconv_qs.emplace_back(
                        this->get_action_id(state_id, action),
                        this->get_action_cost(state_id, action));

                    break;
                }
            }
        }
    }

private:
    /**
     * Initializes the data structures for this new state and pushes it
     * onto the queue unless it is terminal modulo self-loops. Returns
     * true if the state was pushed.
     */
    bool push_state(
        StateID state_id,
        StateInfo& state_info,
        EngineValueType& state_value)
    {
        assert(state_info.status == StateInfo::NEW);

        const State state = this->lookup_state(state_id);

        const TerminationInfo state_eval = this->get_termination_info(state);
        const auto t_cost = state_eval.get_cost();

        const EvaluationResult h_eval = value_initializer_->evaluate(state);
        const auto estimate = h_eval.get_estimate();

        if (state_eval.is_goal_state()) {
            ++statistics_.goal_states;

            if (!expand_goals_) {
                ++statistics_.terminal_states;

                state_value = EngineValueType(t_cost);
                state_info.status = StateInfo::CLOSED;

                return false;
            }
        }

        if (h_eval.is_unsolvable()) {
            ++statistics_.pruned;

            state_value = EngineValueType(estimate);
            state_info.status = StateInfo::CLOSED;

            return false;
        }

        state_info.status = StateInfo::ONSTACK;

        std::vector<Action> aops;
        this->generate_applicable_ops(state_id, aops);
        ++statistics_.expanded_states;

        if (aops.empty()) {
            ++statistics_.terminal_states;

            state_value = EngineValueType(t_cost);
            state_info.status = StateInfo::CLOSED;

            return false;
        }

        /************** Forward to first non self loop ****************/
        Distribution<StateID> transition;

        do {
            Action& current_op = aops.back();

            this->generate_successors(state_id, aops.back(), transition);

            assert(!transition.empty());

            // Check for self loop
            if (!transition.is_dirac(state_id)) {
                if constexpr (UseInterval) {
                    assert(t_cost >= estimate);
                    state_value.lower = estimate;
                    state_value.upper = t_cost;
                } else {
                    state_value = estimate;
                }

                std::size_t stack_size = stack_.size();

                state_info.stack_id = stack_size;

                // Found non self loop action, push and return success.
                auto& s_info = stack_.emplace_back(
                    state_id,
                    state_value,
                    t_cost,
                    aops.size());

                s_info.nconv_qs.emplace_back(
                    this->get_action_id(state_id, current_op),
                    this->get_action_cost(state_id, current_op));

                exploration_stack_.emplace_back(
                    state_id,
                    stack_size,
                    std::move(aops),
                    std::move(transition));

                return true;
            }

            aops.pop_back();
            transition.clear();
        } while (!aops.empty());
        /*****************************************************************/

        state_value = EngineValueType(t_cost);
        state_info.status = StateInfo::CLOSED;

        return false;
    }

    /**
     * Iterates over all possible successors and tries to find a new
     * non-terminal state. If such a state is found, pushes it and
     * return true, otherwise returns false.
     *
     * Initializes the data structures of all newly encountered successor
     * states, and updates the data structures of the source state.
     */
    template <typename ValueStore>
    bool successor_loop(
        ExplorationInfo& explore,
        StackInfo& stack_info,
        StateID state_id,
        ValueStore& value_store)
    {
        for (;;) {
            assert(!stack_info.nconv_qs.empty());

            QValueInfo& tinfo = stack_info.nconv_qs.back();

            do {
                const auto [succ_id, prob] = explore.get_current_successor();

                if (succ_id == state_id) {
                    explore.self_loop_prob += prob;
                    continue;
                }

                StateInfo& succ_info = state_information_[succ_id];
                EngineValueType& s_value = value_store[succ_id];
                int status = succ_info.status;

                if (status == StateInfo::ONSTACK) {
                    explore.update_lowlink(succ_info.stack_id);
                    tinfo.nconv_successors.emplace_back(&s_value, prob);
                } else if (
                    status == StateInfo::NEW &&
                    push_state(succ_id, succ_info, s_value)) {
                    return true; // recursion on new state
                } else {
                    tinfo.conv_part += prob * s_value;
                }
            } while (explore.next_successor());

            if (tinfo.has_no_scc_successors()) {
                if (set_min(stack_info.conv_part, tinfo.conv_part)) {
                    stack_info.best_converged_id = tinfo.action_id;
                }
                stack_info.nconv_qs.pop_back();
            } else {
                tinfo.normalize(explore.self_loop_prob);
            }

            if (!explore.next_action(this, state_id)) {
                return false;
            }

            const Action& action = explore.get_current_action();

            stack_info.nconv_qs.emplace_back(
                this->get_action_id(state_id, action),
                this->get_action_cost(state_id, action));
        }
    }

    using StackIterator = typename std::vector<StackInfo>::iterator;

    /**
     * Handle the new SCC and perform value iteration on it.
     */
    void scc_found(
        StackIterator begin,
        StackIterator end,
        policies::MapPolicy<State, Action>* policy)
    {
        assert(begin != end);

        ++statistics_.sccs;

        if (std::distance(begin, end) == 1) {
            // Singleton SCCs can only transition to a child SCC. The state
            // value has already converged due to topological ordering.
            ++statistics_.singleton_sccs;
            StateInfo& state_info = state_information_[begin->state_id];
            update(*begin->value, begin->conv_part);
            assert(state_info.status == StateInfo::ONSTACK);
            state_info.status = StateInfo::CLOSED;
        } else {
            // Mark all states as closed
            {
                auto it = begin;
                do {
                    StackInfo& stack_it = *it;
                    StateInfo& state_it = state_information_[stack_it.state_id];

                    assert(state_it.status == StateInfo::ONSTACK);
                    assert(!stack_it.nconv_qs.empty());

                    state_it.status = StateInfo::CLOSED;
                } while (++it != end);
            }

            // Now run VI on the SCC until convergence
            bool changed;

            do {
                changed = false;
                auto it = begin;

                do {
                    changed |= it->update_value();
                    ++statistics_.bellman_backups;
                } while (++it != end);
            } while (changed);

            // Extract a policy from this SCC
            if (policy) {
                auto it = begin;

                do {
                    if constexpr (UseInterval) {
                        policy->emplace_decision(
                            it->state_id,
                            it->best_action_id,
                            *it->value);
                    } else {
                        policy->emplace_decision(
                            it->state_id,
                            it->best_action_id,
                            Interval(*it->value, INFINITE_VALUE));
                    }
                } while (++it != end);
            }
        }

        stack_.erase(begin, end);
    }
};

} // namespace topological_vi
} // namespace engines
} // namespace probfd

#endif // __TOPOLOGICAL_VALUE_ITERATION_H__