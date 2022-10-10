#ifndef MDPS_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H
#define MDPS_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H

#include "../../utils/iterators.h"
#include "../storage/per_state_storage.h"
#include "../value_utils.h"
#include "engine.h"

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
public:
    using IncumbentSolution = value_utils::IncumbentSolution<Interval>;
    using Store = storage::PersistentPerStateStorage<IncumbentSolution>;

    explicit TopologicalValueIteration(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        RewardFunction<State, Action>* reward_function,
        value_utils::IntervalValue reward_bound,
        TransitionGenerator<Action>* transition_generator,
        const StateEvaluator<State>* value_initializer,
        bool expand_goals)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              reward_function,
              reward_bound,
              transition_generator)
        , value_initializer_(value_initializer)
        , expand_goals_(expand_goals)
        , dead_end_value_(this->get_minimal_reward())
    {
    }

    /**
     * \copydoc MDPEngine::solve(const State&)
     */
    virtual value_type::value_t solve(const State& state) override
    {
        this->value_store_.reset(new Store());
        this->solve(this->get_state_id(state), *this->value_store_);
        return value_utils::as_upper_bound(
            value_store_->operator[](this->get_state_id(state)));
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

        if (iinfo.status == StateInfo::CLOSED) {
            return value_utils::as_upper_bound(init_value);
        }

        push_state(init_state_id, iinfo, init_value, dead_end_out);

        StateInfo* backtracked_state_info_ = nullptr;

        while (!exploration_stack_.empty()) {
            ExplorationInfo& explore = exploration_stack_.back();

            const StateID& state_id = explore.state_id;

            StackInfo& stack_info = stack_[explore.stackidx];
            StateInfo& state_info = state_information_[state_id];

            assert(state_id == stack_info.state_id);
            assert(state_info.status == StateInfo::ONSTACK);

            if (backtracked_state_info_ != nullptr) {
                state_info.update_lowlink(backtracked_state_info_->lowlink);
                state_info.update_dead(backtracked_state_info_->dead);
                backtracked_state_info_ = nullptr;
            }

            bool recurse = successor_loop(
                explore,
                stack_info,
                state_info,
                state_id,
                value_store,
                dead_end_out);

            if (recurse) {
                continue;
            }

            // Check if an SCC was found.
            if (state_info.dfs_index == state_info.lowlink) {
                const auto begin = stack_.begin() + explore.stackidx;
                const auto end = stack_.end();
                scc_found(state_info, begin, end, dead_end_out);
            }

            backtracked_state_info_ = &state_info;
            exploration_stack_.pop_back();
        }

        return value_utils::as_upper_bound(init_value);
    }

private:
    struct StateInfo {
        // Status Flags
        enum { NEW, OPEN, ONSTACK, CLOSED };

        StateInfo()
            : status(NEW)
            , dead(true)
        {
        }

        uint8_t status : 7;
        uint8_t dead : 1; // Dead-end flag

        // Tarjan's info
        unsigned dfs_index = 0;
        unsigned lowlink = 0;

        void update_lowlink(unsigned upd) { lowlink = std::min(lowlink, upd); }
        void update_dead(unsigned d) { dead = dead && d; }
    };

    struct ExplorationInfo {
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
            // First action was handled already.
            this->aops.pop_back();
        }

        /**
         * Advances to the next non-loop action. Returns nullptr if such an
         * action does not exist.
         */
        const Action*
        next_action(TopologicalValueIteration* self, unsigned int state_id)
        {
            while (!aops.empty()) {
                transition.clear();
                const Action& action = aops.back();
                self->generate_successors(state_id, action, transition);
                transition.make_unique();
                successor = transition.begin();

                if (transition.size() != 1 || successor->first != state_id) {
                    return &action;
                }

                aops.pop_back();
            }

            return nullptr;
        }

        // Immutable info
        StateID state_id;  // State this information belongs to
        unsigned stackidx; // Index on the stack of the associated state

        // Mutable info
        std::vector<Action> aops;         // Remaining unexpanded operators
        Distribution<StateID> transition; // Currently expanded transition
        // Next state to expand
        typename Distribution<StateID>::const_iterator successor;
    };

    struct QValueInfo {
        explicit QValueInfo(value_type::value_t action_reward)
            : conv_part(action_reward)
        {
        }

        bool finalize()
        {
            if (has_self_loop) {
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

            for (auto& [prob, value] : nconv_successors) {
                res += prob * (*value);
            }

            if (has_self_loop) {
                res *= self_loop_prob;
            }

            return res;
        }

        bool has_self_loop = false;

        // Probability to remain in the same state.
        // HACK: Casted to the self-loop normalization factor after finalize().
        value_type::value_t self_loop_prob = value_type::zero;

        // Precomputed part of the Q-value.
        // Sum of action reward plus weighted values for successor states which
        // have already converged due to topological ordering.
        IncumbentSolution conv_part;

        using WeightedValueRef =
            std::pair<value_type::value_t, IncumbentSolution*>;

        // Probability and reference to values for successor states which
        // have not yet converged, self-loops excluded.
        std::vector<WeightedValueRef> nconv_successors;
    };

    struct StackInfo {
        StackInfo(
            const StateID& state_id,
            IncumbentSolution& value_ref,
            IncumbentSolution conv_part,
            unsigned num_aops)
            : state_id(state_id)
            , value(&value_ref)
            , conv_part(conv_part)
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

        StateID state_id;

        // Reference to the state value of the state.
        IncumbentSolution* value;

        // Precomputed part of the max of the value update.
        // Maximum over all Q values which have already converged due to
        // topological ordering.
        IncumbentSolution conv_part;

        // Remaining Q values which have not yet converged.
        std::vector<QValueInfo> nconv_qs;
    };

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

        State state = this->lookup_state(state_id);
        EvaluationResult state_eval = this->get_state_reward(state);

        state_info.dfs_index = state_info.lowlink = index_++;

        if (state_eval) {
            state_value =
                IncumbentSolution(static_cast<value_type::value_t>(state_eval));
            state_info.dead = false;
            ++statistics_.goal_states;

            if (!expand_goals_) {
                ++statistics_.terminal_states;
                goto fail;
            }
        } else {
            auto estimate = value_initializer_->operator()(state);

            if (estimate) {
                ++statistics_.pruned;
                ++statistics_.terminal_states;
                goto fail;
            }

            if constexpr (Interval::value) {
                state_value.lower = this->get_minimal_reward();
                state_value.upper = static_cast<value_type::value_t>(estimate);
            } else {
                state_value = static_cast<value_type::value_t>(estimate);
            }
        }

        state_info.status = StateInfo::ONSTACK;

        {
            std::vector<Action> aops;
            this->generate_applicable_ops(state_id, aops);
            ++statistics_.expanded_states;

            if (aops.empty()) {
                ++statistics_.terminal_states;
                goto fail;
            }

            /************** Forward to first non self loop ****************/
            Distribution<StateID> transition;

            do {
                this->generate_successors(state_id, aops.back(), transition);
                transition.make_unique();

                assert(!transition.empty());

                // Check for self loop
                if (!transition.is_dirac(state_id)) {
                    std::size_t stack_size = stack_.size();

                    // Found non self loop action, push and return success.
                    auto& s_info = stack_.emplace_back(
                        state_id,
                        state_value,
                        state_eval
                            ? IncumbentSolution(
                                  static_cast<value_type::value_t>(state_eval))
                            : dead_end_value_,
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
        }

    fail:
        if (state_info.dead) {
            state_value = dead_end_value_;
            ++statistics_.dead_ends;
            *dead_end_out = state_id;
        }

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
        do {
            assert(!stack_info.nconv_qs.empty());

            QValueInfo& tinfo = stack_info.nconv_qs.back();

            do {
                const auto [succ_id, prob] = *explore.successor;

                if (succ_id == state_id) {
                    tinfo.has_self_loop = true;
                    tinfo.self_loop_prob += prob;
                    continue;
                }

                StateInfo& succ_info = state_information_[succ_id];
                IncumbentSolution& s_value = value_store[succ_id];
                int status = succ_info.status;

                if (status == StateInfo::ONSTACK) {
                    state_info.update_lowlink(succ_info.dfs_index);
                    tinfo.nconv_successors.emplace_back(prob, &s_value);
                } else if (
                    status == StateInfo::NEW &&
                    push_state(succ_id, succ_info, s_value, dead_end_out)) {
                    return true; // recursion on new state
                } else {
                    tinfo.conv_part += prob * s_value;
                    state_info.update_dead(succ_info.dead);
                }
            } while (++explore.successor != explore.transition.end());

            if (tinfo.finalize()) {
                value_utils::set_max(stack_info.conv_part, tinfo.conv_part);
                stack_info.nconv_qs.pop_back();
            }

            const Action* next_action = explore.next_action(this, state_id);

            if (!next_action) {
                return false;
            }

            explore.aops.pop_back();

            stack_info.nconv_qs.emplace_back(
                this->get_action_reward(state_id, *next_action));
        } while (true);
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

                *stack_it.value = dead_end_value_;
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
            auto it = begin;
            do {
                StackInfo& stack_it = *it;
                StateInfo& state_it = state_information_[stack_it.state_id];

                assert(state_it.status == StateInfo::ONSTACK);
                assert(!stack_it.nconv_qs.empty());

                state_it.dead = false;

                state_it.status = StateInfo::CLOSED;
            } while (++it != end);

            // Now run VI on the SCC until convergence
            statistics_.bellman_backups += value_iteration(begin, end);
        }

        stack_.erase(begin, end);
    }

    /**
     * Performs value iteration on the SCC until convergence. Returns the
     * number of performed updates.
     */
    template <typename StackInfoIterator>
    unsigned
    value_iteration(StackInfoIterator begin, StackInfoIterator end) const
    {
        assert(begin != end);

        bool changed;
        unsigned num_updates = 0;

        do {
            changed = false;
            auto it = begin;

            do {
                changed |= it->update_value();
                ++num_updates;
            } while (++it != end);
        } while (changed);

        return num_updates;
    }

    const StateEvaluator<State>* value_initializer_;
    const bool expand_goals_;
    const IncumbentSolution dead_end_value_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::unique_ptr<Store> value_store_;

    unsigned index_ = 0;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    Statistics statistics_;
};

} // namespace topological_vi
} // namespace engines
} // namespace probfd

#endif // __TOPOLOGICAL_VALUE_ITERATION_H__