#ifndef MDPS_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H
#define MDPS_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H

#include "probfd/engines/engine.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/value_utils.h"

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
    unsigned long long dead_ends = 0;
    unsigned long long sccs = 0;
    unsigned long long singleton_sccs = 0;
    unsigned long long bellman_backups = 0;
    unsigned long long pruned = 0;

    void print(std::ostream& out) const
    {
        out << "  Expanded state(s): " << expanded_states << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
        out << "  Dead end state(s): " << dead_ends << std::endl;
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
 * This implementation also supports interval bounds. However, convergence
 * is not guaranteed for interval bounds if traps exist within the reachable
 * state space. In this case, traps must be removed prior to running topological
 * value iteration.
 *
 * @see interval_iteration::IntervalIteration
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam Interval - Whether bounded value iteration is used.
 */
template <typename State, typename Action, typename Interval = std::false_type>
class TopologicalValueIteration : public MDPEngine<State, Action> {
    using IncumbentSolution = value_utils::IncumbentSolution<Interval>;

    struct StateInfo {
        // Status Flags
        enum { NEW, OPEN, ONSTACK, CLOSED };
        
        uint8_t status : 7;
        uint8_t dead : 1; // Dead-end flag

        // Tarjan's info
        unsigned stack_id = 0;
        unsigned lowlink = 0;

        StateInfo()
            : status(NEW)
            , dead(true)
        {
        }

        void update_lowlink(unsigned upd) { lowlink = std::min(lowlink, upd); }
        void update_dead(unsigned d) { dead = dead && d; }
    };

    struct ExplorationInfo {
        // Immutable info
        StateID state_id;  // State this information belongs to
        unsigned stackidx; // Index on the stack of the associated state

        // Mutable info
        std::vector<Action> aops;         // Remaining unexpanded operators
        Distribution<StateID> transition; // Currently expanded transition
        // Next state to expand
        typename Distribution<StateID>::const_iterator successor;

        ExplorationInfo(
            StateID state_id,
            unsigned stackidx,
            std::vector<Action> aops,
            Distribution<StateID> transition)
            : state_id(state_id)
            , stackidx(stackidx)
            , aops(std::move(aops))
            , transition(std::move(transition))
            , successor(this->transition.begin())
        {
        }

        /**
         * Advances to the next non-loop action. Returns nullptr if such an
         * action does not exist.
         */
        bool next_action(TopologicalValueIteration* self, unsigned int state_id)
        {
            for (aops.pop_back(); !aops.empty(); aops.pop_back()) {
                transition.clear();
                self->generate_successors(state_id, aops.back(), transition);
                transition.make_unique();

                if (!transition.is_dirac(state_id)) {
                    successor = transition.begin();
                    return true;
                }
            }

            return false;
        }

        bool next_successor() { return ++successor != transition.end(); }

        Action& get_current_action() { return aops.back(); }

        WeightedElement<StateID> get_current_successor() { return *successor; }
    };

    struct QValueInfo {
        // Probability to remain in the same state.
        // Casted to the self-loop normalization factor after finalize().
        value_type::value_t self_loop_prob = value_type::zero;

        // Precomputed part of the Q-value.
        // Sum of action reward plus those weighted successor values which
        // have already converged due to topological ordering.
        IncumbentSolution conv_part;

        // Pointers to successor values which have not yet converged,
        // self-loops excluded.
        std::vector<WeightedElement<IncumbentSolution*>> nconv_successors;

        explicit QValueInfo(value_type::value_t action_reward)
            : conv_part(action_reward)
        {
        }

        bool finalize()
        {
            if (self_loop_prob != value_type::zero) {
                // Calculate self-loop normalization factor
                self_loop_prob =
                    value_type::one / (value_type::one - self_loop_prob);

                if (nconv_successors.empty()) {
                    // Apply self-loop normalization immediately
                    conv_part *= self_loop_prob;
                }
            }

            return nconv_successors.empty();
        }

        IncumbentSolution compute_q_value() const
        {
            IncumbentSolution res = conv_part;

            for (auto& [value, prob] : nconv_successors) {
                res += prob * (*value);
            }

            if (self_loop_prob != value_type::zero) {
                res *= self_loop_prob;
            }

            return res;
        }
    };

    struct StackInfo {
        StateID state_id;

        // Reference to the state value of the state.
        IncumbentSolution* value;

        // The state reward
        IncumbentSolution state_reward;

        // Precomputed part of the max of the value update.
        // Maximum over all Q values which have already converged due to
        // topological ordering.
        IncumbentSolution conv_part;

        // Remaining Q values which have not yet converged.
        std::vector<QValueInfo> nconv_qs;

        StackInfo(
            const StateID& state_id,
            IncumbentSolution& value_ref,
            value_type::value_t state_reward,
            unsigned num_aops)
            : state_id(state_id)
            , value(&value_ref)
            , state_reward(state_reward)
            , conv_part(state_reward)
        {
            nconv_qs.reserve(num_aops);
        }

        bool update_value()
        {
            IncumbentSolution v = conv_part;

            for (const QValueInfo& info : nconv_qs) {
                value_utils::set_max(v, info.compute_q_value());
            }

            if constexpr (Interval::value) {
                return value_utils::update(*value, v) || !value->bounds_equal();
            } else {
                return value_utils::update(*value, v);
            }
        }
    };

public:
    using Store = storage::PersistentPerStateStorage<IncumbentSolution>;

    explicit TopologicalValueIteration(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* reward_function,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        const engine_interfaces::StateEvaluator<State>* value_initializer,
        bool expand_goals)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              reward_function,
              transition_generator)
        , value_initializer_(value_initializer)
        , expand_goals_(expand_goals)
    {
    }

    /**
     * \copydoc MDPEngine::solve(const State&)
     */
    virtual value_type::value_t solve(const State& state) override
    {
        Store value_store;
        return this->solve(this->get_state_id(state), value_store);
    }

    /**
     * \copydoc MDPEngineInterface::print_statistics(std::ostream&) const
     */
    virtual void print_statistics(std::ostream& out) const override
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
     * output parameter \p value_store and optionally notifies the caller
     * of encountered dead ends via the output iterator \p dead_end_out.
     * Returns the value of the initial state.
     */
    template <
        typename ValueStore,
        typename DeadendOutputIt = utils::discarding_output_iterator>
    value_type::value_t solve(
        StateID init_state_id,
        ValueStore& value_store,
        DeadendOutputIt dead_end_out = DeadendOutputIt{})
    {
        StateInfo& iinfo = state_information_[init_state_id];
        IncumbentSolution& init_value = value_store[init_state_id];

        if (!push_state(init_state_id, iinfo, init_value, dead_end_out)) {
            return value_utils::as_upper_bound(init_value);
        }

        ExplorationInfo* explore = &exploration_stack_.back();
        StateID state_id = explore->state_id;
        StackInfo* stack_info = &stack_[explore->stackidx];
        StateInfo* state_info = &state_information_[state_id];

        for (;;) {
            while (successor_loop(
                *explore,
                *stack_info,
                *state_info,
                state_id,
                value_store,
                dead_end_out)) {
                explore = &exploration_stack_.back();
                state_id = explore->state_id;
                stack_info = &stack_[explore->stackidx];
                state_info = &state_information_[state_id];
            }

            for (;;) {
                // Check if an SCC was found.
                const unsigned stack_id = state_info->stack_id;
                const unsigned lowlink = state_info->lowlink;
                const bool onstack = stack_id != lowlink;

                if (!onstack) {
                    const auto begin = stack_.begin() + stack_id;
                    const auto end = stack_.end();
                    scc_found(*state_info, begin, end, dead_end_out);
                }

                exploration_stack_.pop_back();

                if (exploration_stack_.empty()) {
                    goto break_exploration;
                }

                StateInfo* bt_state_info = state_info;

                explore = &exploration_stack_.back();
                state_id = explore->state_id;
                stack_info = &stack_[explore->stackidx];
                state_info = &state_information_[state_id];

                const auto [succ_id, prob] = explore->get_current_successor();
                IncumbentSolution& s_value = value_store[succ_id];
                QValueInfo& tinfo = stack_info->nconv_qs.back();

                if (onstack) {
                    state_info->update_lowlink(bt_state_info->lowlink);
                    tinfo.nconv_successors.emplace_back(&s_value, prob);
                } else {
                    tinfo.conv_part += prob * s_value;
                }

                state_info->update_dead(bt_state_info->dead);

                if (explore->next_successor()) {
                    break;
                }

                if (tinfo.finalize()) {
                    value_utils::set_max(
                        stack_info->conv_part,
                        tinfo.conv_part);
                    stack_info->nconv_qs.pop_back();
                }

                if (explore->next_action(this, state_id)) {
                    stack_info->nconv_qs.emplace_back(this->get_action_reward(
                        state_id,
                        explore->get_current_action()));

                    break;
                }
            }
        }

    break_exploration:;

        return value_utils::as_upper_bound(init_value);
    }

private:
    /**
     * Initializes the data structures for this new state and pushes it
     * onto the queue unless it is terminal modulo self-loops. Returns
     * true if the state was pushed.
     */
    template <typename DeadendOutputIt>
    bool push_state(
        StateID state_id,
        StateInfo& state_info,
        IncumbentSolution& state_value,
        DeadendOutputIt dead_end_out)
    {
        assert(state_info.status == StateInfo::NEW);

        const State state = this->lookup_state(state_id);

        const EvaluationResult state_eval = this->get_state_reward(state);
        const auto t_reward = static_cast<value_type::value_t>(state_eval);

        const EvaluationResult h_eval = value_initializer_->operator()(state);
        const auto estimate = static_cast<value_type::value_t>(h_eval);

        if (state_eval) {
            state_info.dead = false;
            ++statistics_.goal_states;

            if (!expand_goals_) {
                ++statistics_.terminal_states;

                state_value = IncumbentSolution(t_reward);
                state_info.status = StateInfo::CLOSED;

                return false;
            }
        }

        if (h_eval) {
            ++statistics_.pruned;

            state_value = IncumbentSolution(estimate);
            state_info.status = StateInfo::CLOSED;

            return false;
        }

        state_info.status = StateInfo::ONSTACK;

        std::vector<Action> aops;
        this->generate_applicable_ops(state_id, aops);
        ++statistics_.expanded_states;

        if (aops.empty()) {
            ++statistics_.terminal_states;
            ++statistics_.dead_ends;

            *dead_end_out = state_id;

            state_value = IncumbentSolution(t_reward);
            state_info.status = StateInfo::CLOSED;

            return false;
        }

        /************** Forward to first non self loop ****************/
        Distribution<StateID> transition;

        do {
            this->generate_successors(state_id, aops.back(), transition);
            transition.make_unique();

            assert(!transition.empty());

            // Check for self loop
            if (!transition.is_dirac(state_id)) {
                if constexpr (Interval::value) {
                    assert(t_reward <= estimate);
                    state_value.lower = t_reward;
                    state_value.upper = estimate;
                } else {
                    state_value = estimate;
                }

                std::size_t stack_size = stack_.size();

                state_info.stack_id = state_info.lowlink = stack_size;

                // Found non self loop action, push and return success.
                auto& s_info = stack_.emplace_back(
                    state_id,
                    state_value,
                    t_reward,
                    aops.size());

                s_info.nconv_qs.emplace_back(
                    this->get_action_reward(state_id, aops.back()));

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

        if (state_info.dead) {
            ++statistics_.dead_ends;
            *dead_end_out = state_id;
        }

        state_value = IncumbentSolution(t_reward);
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
    template <typename ValueStore, typename DeadendOutputIt>
    bool successor_loop(
        ExplorationInfo& explore,
        StackInfo& stack_info,
        StateInfo& state_info,
        const StateID& state_id,
        ValueStore& value_store,
        DeadendOutputIt dead_end_out)
    {
        for (;;) {
            assert(!stack_info.nconv_qs.empty());

            QValueInfo& tinfo = stack_info.nconv_qs.back();

            do {
                const auto [succ_id, prob] = explore.get_current_successor();

                if (succ_id == state_id) {
                    tinfo.self_loop_prob += prob;
                    continue;
                }

                StateInfo& succ_info = state_information_[succ_id];
                IncumbentSolution& s_value = value_store[succ_id];
                int status = succ_info.status;

                if (status == StateInfo::ONSTACK) {
                    state_info.update_lowlink(succ_info.stack_id);
                    tinfo.nconv_successors.emplace_back(&s_value, prob);
                } else if (
                    status == StateInfo::NEW &&
                    push_state(succ_id, succ_info, s_value, dead_end_out)) {
                    return true; // recursion on new state
                } else {
                    tinfo.conv_part += prob * s_value;
                    state_info.update_dead(succ_info.dead);
                }
            } while (explore.next_successor());

            if (tinfo.finalize()) {
                value_utils::set_max(stack_info.conv_part, tinfo.conv_part);
                stack_info.nconv_qs.pop_back();
            }

            if (!explore.next_action(this, state_id)) {
                return false;
            }

            stack_info.nconv_qs.emplace_back(this->get_action_reward(
                state_id,
                explore.get_current_action()));
        }
    }

    /**
     * Handle the new SCC and perform value iteration on it.
     */
    template <typename StackIterator, typename DeadendOutputIt>
    void scc_found(
        StateInfo& state_info,
        StackIterator begin,
        StackIterator end,
        DeadendOutputIt dead_end_out)
    {
        assert(begin != end);

        ++statistics_.sccs;

        const auto scc_size = end - begin;

        if (scc_size == 1) {
            ++statistics_.singleton_sccs;
        }

        if (state_info.dead) {
            // If the SCC consists of dead ends, report them and set their
            // state values accordingly
            auto it = begin;
            do {
                StackInfo& stack_it = *it;
                StateInfo& state_it = state_information_[stack_it.state_id];

                assert(state_it.dead && state_it.status == StateInfo::ONSTACK);

                *stack_it.value = IncumbentSolution(stack_it.state_reward);
                *dead_end_out = stack_it.state_id;

                state_it.status = StateInfo::CLOSED;
            } while (++it != end);

            statistics_.dead_ends += scc_size;
        } else if (scc_size == 1) {
            // For singleton SCCs, we only have transitions which are
            // self-loops or go to a state that is topologically greater.
            // The state value is therefore the base value.
            assert(state_info.status == StateInfo::ONSTACK);
            value_utils::update(*begin->value, begin->conv_part);
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

                    state_it.dead = false;

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
        }

        stack_.erase(begin, end);
    }

    const engine_interfaces::StateEvaluator<State>* value_initializer_;
    const bool expand_goals_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    Statistics statistics_;
};

} // namespace topological_vi
} // namespace engines
} // namespace probfd

#endif // __TOPOLOGICAL_VALUE_ITERATION_H__