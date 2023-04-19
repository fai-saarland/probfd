#ifndef PROBFD_ENGINES_TA_TOPOLOGICAL_VALUE_ITERATION_H
#define PROBFD_ENGINES_TA_TOPOLOGICAL_VALUE_ITERATION_H

#include "probfd/engines/engine.h"
#include "probfd/engines/utils.h"

#include "probfd/engines/utils.h"

#include "probfd/storage/per_state_storage.h"

#include "utils/countdown_timer.h"
#include "utils/iterators.h"

#include <deque>
#include <iostream>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

/// Namespace dedicated to Topological Value Iteration (TVI).
namespace ta_topological_vi {

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
 * @brief Implements a trap-aware variant of Topological Value Iteration
 * \cite dai:etal:jair-11.
 *
 * Topological value iteration computes the SCCs of the MDP and performs value
 * iteration until epsilon convergence on each SCC in reverse topological order.
 * The topological ordering yields an optimal update order since a parent SCC's
 * value depends on its child SCCs, but not vice versa.
 *
 * Additionally, this implementation computes all traps reachable from the
 * initial state on-the-fly and flattens them to guarantee convergence against
 * the optimal value function. Traps need not be removed prior to running
 * the algorithm. UseInterval bounds are also supported.
 *
 * @see topological_vi:TopologicalValueIteration
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam UseInterval - Whether bounded value iteration is used.
 */
template <typename State, typename Action, bool UseInterval = false>
class TATopologicalValueIteration : public MDPEngine<State, Action> {
    using EngineValueType = engines::EngineValueType<UseInterval>;

    struct StateInfo {
        // Status Flags
        enum { NEW, ONSTACK, CLOSED };

        uint8_t status = NEW;
        unsigned stack_id = std::numeric_limits<unsigned>::max();
        unsigned ecd_stack_id = std::numeric_limits<unsigned>::max();
    };

    struct ExplorationInfo {
        // Tarjans's algorithm state
        unsigned stackidx;
        unsigned lowlink;

        // Exploration State -- Remaining operators
        std::vector<Action> aops;

        // Exploration State -- Currently expanded transition and successor
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;

        // End component decomposition state

        // recursive decomposition flag
        // Recursively decompose the SCC if there is a zero-cost transition
        // in it that can leave and remain in the scc.
        bool recurse = false;

        // whether the transition has non-zero cost or can leave the scc
        bool nz_or_leaves_scc;

        ExplorationInfo(
            unsigned stackidx,
            std::vector<Action> aops,
            Distribution<StateID> transition)
            : stackidx(stackidx)
            , lowlink(stackidx)
            , aops(std::move(aops))
            , transition(std::move(transition))
            , successor(this->transition.begin())
        {
        }

        /**
         * Advances to the next non-loop action. Returns nullptr if such an
         * action does not exist.
         */
        bool
        next_action(TATopologicalValueIteration* self, unsigned int state_id)
        {
            for (aops.pop_back(); !aops.empty(); aops.pop_back()) {
                transition.clear();
                self->generate_successors(state_id, aops.back(), transition);

                if (!transition.is_dirac(state_id)) {
                    successor = transition.begin();
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
        // Probability to remain in the same state.
        // Casted to the self-loop normalization factor after finalize().
        value_t self_loop_prob = 0.0_vt;

        // Precomputed part of the Q-value.
        // Sum of action cost plus those weighted successor values which
        // have already converged due to topological ordering.
        EngineValueType conv_part;

        // Pointers to successor values which have not yet converged,
        // self-loops excluded.
        std::vector<ItemProbabilityPair<StateID>> scc_successors;

        explicit QValueInfo(value_t action_cost)
            : conv_part(action_cost)
        {
        }

        bool finalize()
        {
            if (self_loop_prob != 0.0_vt) {
                // Calculate self-loop normalization factor
                self_loop_prob = 1.0_vt / (1.0_vt - self_loop_prob);

                if (scc_successors.empty()) {
                    // Apply self-loop normalization immediately
                    conv_part *= self_loop_prob;
                }
            }

            return scc_successors.empty();
        }

        template <typename ValueStore>
        EngineValueType compute_q_value(ValueStore& value_store) const
        {
            EngineValueType res = conv_part;

            for (auto& [state_id, prob] : scc_successors) {
                res += prob * value_store[state_id];
            }

            if (self_loop_prob != 0.0_vt) {
                res *= self_loop_prob;
            }

            return res;
        }
    };

    struct StackInfo {
        StateID state_id;

        // Reference to the state value of the state.
        EngineValueType* value;

        // Precomputed portion of the Bellman update.
        // Maximum over all Q values for actions which always
        // leave the current scc.
        EngineValueType conv_part;

        // Q value structs for transitions belonging to the scc,
        // but not to an end component.
        std::vector<QValueInfo> non_ec_transitions;

        // Q value structs for transitions currently assumed to belong
        // to an end component within the current scc.
        // Iteratively refined during end component decomposition.
        std::vector<QValueInfo> ec_transitions;

        StackInfo(
            StateID state_id,
            EngineValueType& value_ref,
            value_t state_cost,
            unsigned num_aops)
            : state_id(state_id)
            , value(&value_ref)
            , conv_part(state_cost)
        {
            non_ec_transitions.reserve(num_aops);
            ec_transitions.reserve(num_aops);
        }

        template <typename ValueStore>
        bool update_value(ValueStore& value_store)
        {
            EngineValueType v = conv_part;

            for (const QValueInfo& info : non_ec_transitions) {
                set_min(v, info.compute_q_value(value_store));
            }

            if constexpr (UseInterval) {
                return update(*value, v) || !value->bounds_equal();
            } else {
                return update(*value, v);
            }
        }
    };

    struct ECDExplorationInfo {
        // Tarjan's algorithm state
        unsigned stackidx;
        unsigned lowlink;

        // Reference to the value update struct for this state.
        StackInfo& stack_info;

        // Exploration state - Action
        typename std::vector<QValueInfo>::iterator action;
        typename std::vector<QValueInfo>::iterator end;

        // Exploration state - Transition successor
        typename std::vector<ItemProbabilityPair<StateID>>::iterator successor;

        // End component decomposition state

        // ECD recursion flag. Recurse if there is a transition that can leave
        // and remain in the current scc.
        bool recurse = false;

        // Whether the current transition remains in or leaves the current scc.
        bool leaves_scc = false;
        bool remains_scc = false;

        ECDExplorationInfo(unsigned stackidx, StackInfo& stack_info)
            : stackidx(stackidx)
            , lowlink(stackidx)
            , stack_info(stack_info)
            , action(stack_info.ec_transitions.begin())
            , end(stack_info.ec_transitions.end())
            , successor(action->scc_successors.begin())
        {
            assert(!stack_info.ec_transitions.empty());
            assert(!action->scc_successors.empty());
            assert(action < end);
        }

        bool next_action()
        {
            assert(!action->scc_successors.empty());
            assert(action < end);

            if (!leaves_scc) {
                if (++action != end) {
                    successor = action->scc_successors.begin();
                    assert(!action->scc_successors.empty());

                    remains_scc = false;

                    return true;
                }
            } else {
                stack_info.non_ec_transitions.push_back(std::move(*action));

                if (action != --end) {
                    *action = std::move(*end);
                    assert(!action->scc_successors.empty());
                    successor = action->scc_successors.begin();

                    leaves_scc = false;
                    remains_scc = false;

                    return true;
                }
            }

            auto& ect = stack_info.ec_transitions;
            ect.erase(end, ect.end());

            return false;
        }

        bool next_successor()
        {
            assert(!action->scc_successors.empty());
            return ++successor != action->scc_successors.end();
        }

        QValueInfo& get_current_action() { return *action; }

        ItemProbabilityPair<StateID> get_current_successor()
        {
            return *successor;
        }
    };

    const engine_interfaces::Evaluator<State>* value_initializer_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    std::deque<ECDExplorationInfo> exploration_stack_ecd_;
    std::deque<StateID> stack_ecd_;

    Statistics statistics_;

public:
    TATopologicalValueIteration(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        const engine_interfaces::Evaluator<State>* value_initializer)
        : MDPEngine<State, Action>(state_space, cost_function)
        , value_initializer_(value_initializer)
    {
    }

    /**
     * \copydoc MDPEngine::solve(const State&)
     */
    Interval solve(const State& state, double max_time) override
    {
        storage::PerStateStorage<EngineValueType> value_store;
        return this->solve(this->get_state_id(state), value_store, max_time);
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
        double max_time = std::numeric_limits<double>::infinity())
    {
        utils::CountdownTimer timer(max_time);

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
        StackInfo* stack_info = &stack_[explore->stackidx];
        StateID state_id = stack_info->state_id;

        for (;;) {
            while (successor_loop(
                *explore,
                *stack_info,
                state_id,
                value_store,
                timer)) {
                explore = &exploration_stack_.back();
                stack_info = &stack_[explore->stackidx];
                state_id = stack_info->state_id;
            }

            // Iterative backtracking
            for (;;) {
                const unsigned stack_id = explore->stackidx;
                const unsigned lowlink = explore->lowlink;
                const bool onstack = stack_id != lowlink;

                // Check if an SCC was found.
                if (!onstack) {
                    scc_found(
                        value_store,
                        *explore,
                        stack_.begin() + stack_id,
                        stack_.end(),
                        timer);
                }

                const bool recurse = explore->recurse;

                exploration_stack_.pop_back();

                if (exploration_stack_.empty()) {
                    goto break_exploration;
                }

                timer.throw_if_expired();

                explore = &exploration_stack_.back();
                stack_info = &stack_[explore->stackidx];
                state_id = stack_info->state_id;

                const auto [succ_id, prob] = explore->get_current_successor();

                QValueInfo& tinfo = stack_info->ec_transitions.back();

                if (onstack) {
                    explore->lowlink = std::min(explore->lowlink, lowlink);
                    explore->recurse = explore->recurse || recurse ||
                                       explore->nz_or_leaves_scc;

                    tinfo.scc_successors.emplace_back(succ_id, prob);
                } else {
                    const EngineValueType& s_value = value_store[succ_id];
                    explore->recurse =
                        explore->recurse || !tinfo.scc_successors.empty();
                    explore->nz_or_leaves_scc = true;

                    tinfo.conv_part += prob * s_value;
                }

                // Has next successor -> stop backtracking
                if (explore->next_successor()) {
                    break;
                }

                // Check whether the transition is part of the scc or an end
                // component within the scc
                if (tinfo.finalize()) {
                    // Universally exiting -> Not part of scc
                    // Update converged portion of q value and ignore this
                    // transition
                    set_min(stack_info->conv_part, tinfo.conv_part);
                    stack_info->ec_transitions.pop_back();
                } else if (explore->nz_or_leaves_scc) {
                    // Only some exiting or cost is non-zero ->
                    // Not part of an end component
                    // Move the transition to the set of non-EC transitions
                    stack_info->non_ec_transitions.push_back(std::move(tinfo));
                    stack_info->ec_transitions.pop_back();
                }

                // Has next action -> stop backtracking
                if (explore->next_action(this, state_id)) {
                    const auto cost = this->get_action_cost(
                        state_id,
                        explore->get_current_action());
                    explore->nz_or_leaves_scc = cost != 0.0_vt;
                    stack_info->ec_transitions.emplace_back(cost);

                    break;
                }
            }
        }

    break_exploration:;

        if constexpr (UseInterval) {
            return init_value;
        } else {
            return Interval(init_value, INFINITE_VALUE);
        }
    }

private:
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
        ValueStore& value_store,
        utils::CountdownTimer& timer)
    {
        for (;;) {
            assert(!stack_info.ec_transitions.empty());

            QValueInfo& tinfo = stack_info.ec_transitions.back();

            do {
                timer.throw_if_expired();

                const auto [succ_id, prob] = explore.get_current_successor();

                if (succ_id == state_id) {
                    tinfo.self_loop_prob += prob;
                    continue;
                }

                StateInfo& succ_info = state_information_[succ_id];
                EngineValueType& s_value = value_store[succ_id];
                int status = succ_info.status;

                switch (status) {
                case StateInfo::NEW:
                    if (push_state(succ_id, succ_info, s_value)) {
                        return true; // recursion on new state
                    }

                    [[fallthrough]];
                default:
                    explore.nz_or_leaves_scc = true;
                    explore.recurse =
                        explore.recurse || !tinfo.scc_successors.empty();

                    tinfo.conv_part += prob * s_value;
                    break;

                case StateInfo::ONSTACK:
                    explore.lowlink =
                        std::min(explore.lowlink, succ_info.stack_id);
                    explore.recurse =
                        explore.recurse || explore.nz_or_leaves_scc;

                    tinfo.scc_successors.emplace_back(succ_id, prob);
                }
            } while (explore.next_successor());

            if (tinfo.finalize()) {
                set_min(stack_info.conv_part, tinfo.conv_part);
                stack_info.ec_transitions.pop_back();
            } else if (explore.nz_or_leaves_scc) {
                stack_info.non_ec_transitions.push_back(std::move(tinfo));
                stack_info.ec_transitions.pop_back();
            }

            if (!explore.next_action(this, state_id)) {
                return false;
            }

            const auto cost =
                this->get_action_cost(state_id, explore.get_current_action());

            explore.nz_or_leaves_scc = cost != 0.0_vt;
            stack_info.ec_transitions.emplace_back(cost);
        }
    }

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

        const TerminationInfo state_term = this->get_termination_info(state);
        const auto t_cost = state_term.get_cost();

        if (state_term.is_goal_state()) {
            ++statistics_.goal_states;
        }

        const EvaluationResult h_eval = value_initializer_->evaluate(state);
        const auto estimate = h_eval.get_estimate();

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
            this->generate_successors(state_id, aops.back(), transition);

            assert(!transition.empty());

            // Check for self loop
            if (!transition.is_dirac(state_id)) {
                std::size_t stack_size = stack_.size();

                state_info.stack_id = stack_size;

                // Found non self loop action, push and return success.
                const auto cost =
                    this->get_action_cost(state_id, aops.back());

                ExplorationInfo& explore = exploration_stack_.emplace_back(
                    stack_size,
                    std::move(aops),
                    std::move(transition));

                explore.nz_or_leaves_scc = cost != 0.0_vt;

                auto& s_info = stack_.emplace_back(
                    state_id,
                    state_value,
                    t_cost,
                    aops.size());

                s_info.ec_transitions.emplace_back(cost);

                if constexpr (UseInterval) {
                    state_value.lower = t_cost;
                    state_value.upper = estimate;
                } else {
                    state_value = estimate;
                }

                return true;
            }

            aops.pop_back();
            transition.clear();
        } while (!aops.empty());

        state_value = EngineValueType(t_cost);
        state_info.status = StateInfo::CLOSED;

        return false;
    }

    /**
     * Handle the new SCC and perform value iteration on it.
     */
    template <typename ValueStore, typename StackIterator>
    void scc_found(
        ValueStore& value_store,
        ExplorationInfo& exp_info,
        StackIterator begin,
        StackIterator end,
        utils::CountdownTimer& timer)
    {
        assert(begin != end);

        ++statistics_.sccs;

        if (std::distance(end, begin) == 1) {
            // For singleton SCCs, we only have transitions which are
            // self-loops or go to a state that is topologically greater.
            // The state value is therefore the base value.
            StateInfo& state_info = state_information_[begin->state_id];
            assert(state_info.status == StateInfo::ONSTACK);
            update(*begin->value, begin->conv_part);
            state_info.status = StateInfo::CLOSED;
            ++statistics_.singleton_sccs;
            stack_.pop_back();
            return;
        }

        if (exp_info.recurse) {
            // Run recursive EC Decomposition
            for (auto it = begin; it != end; ++it) {
                state_information_[it->state_id].status = StateInfo::NEW;
            }

            for (auto it = begin; it != end; ++it) {
                const StateID id = it->state_id;
                StateInfo& s_info = state_information_[id];

                if (s_info.status == StateInfo::NEW && push_ecd(id, s_info)) {
                    // Run decomposition
                    find_and_decompose_sccs(0, timer);
                }

                assert(s_info.status == StateInfo::CLOSED);
            }

            assert(exploration_stack_ecd_.empty());
        } else {
            // We found an end component, patch it
            const StateID scc_repr_id = begin->state_id;
            StateInfo& repr_state_info = state_information_[scc_repr_id];
            StackInfo& repr_stk = stack_[repr_state_info.stack_id];

            repr_state_info.status = StateInfo::CLOSED;

            // Spider construction
            for (auto it = begin + 1; it != end; ++it) {
                StateInfo& state_info = state_information_[it->state_id];
                StackInfo& succ_stk = stack_[state_info.stack_id];

                state_info.status = StateInfo::CLOSED;

                // Move all non-EC transitions to representative state
                std::move(
                    succ_stk.non_ec_transitions.begin(),
                    succ_stk.non_ec_transitions.end(),
                    std::back_inserter(repr_stk.non_ec_transitions));

                // Free memory
                std::vector<QValueInfo>().swap(succ_stk.non_ec_transitions);

                set_min(repr_stk.conv_part, succ_stk.conv_part);

                succ_stk.conv_part = EngineValueType(INFINITE_VALUE);

                // Connect to representative state with zero cost action
                auto& t = succ_stk.non_ec_transitions.emplace_back(0.0_vt);
                t.scc_successors.emplace_back(scc_repr_id, 1.0_vt);
            }
        }

        // Now run VI on the SCC until convergence
        bool changed;

        do {
            timer.throw_if_expired();

            changed = false;
            auto it = begin;

            do {
                changed |= it->update_value(value_store);
                ++statistics_.bellman_backups;
            } while (++it != end);
        } while (changed);

        stack_.erase(begin, end);
    }

    void
    find_and_decompose_sccs(const unsigned limit, utils::CountdownTimer& timer)
    {
        if (exploration_stack_ecd_.size() <= limit) {
            return;
        }

        ECDExplorationInfo* e = &exploration_stack_ecd_.back();

        for (;;) {
            // DFS recursion
            while (push_successor_ecd(*e, timer)) {
                e = &exploration_stack_ecd_.back();
            }

            // Iterative backtracking
            do {
                const bool recurse = e->recurse;
                const unsigned int stck = e->stackidx;
                const unsigned int lowlink = e->lowlink;

                assert(stck >= lowlink);

                const bool is_onstack = stck != lowlink;

                if (!is_onstack) {
                    scc_found_ecd(*e, timer);
                }

                exploration_stack_ecd_.pop_back();

                if (exploration_stack_ecd_.size() <= limit) {
                    return;
                }

                timer.throw_if_expired();

                e = &exploration_stack_ecd_.back();

                if (is_onstack) {
                    e->lowlink = std::min(e->lowlink, lowlink);
                    e->recurse = e->recurse || recurse || e->leaves_scc;
                    e->remains_scc = true;
                } else {
                    e->recurse = e->recurse || e->remains_scc;
                    e->leaves_scc = true;
                }
            } while (!e->next_successor() && !e->next_action());
        }
    }

    bool push_successor_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer)
    {
        do {
            timer.throw_if_expired();

            const StateID succ_id = e.get_current_successor().item;
            StateInfo& succ_info = state_information_[succ_id];

            if (succ_info.status == StateInfo::NEW) {
                if (push_ecd(succ_id, succ_info)) {
                    return true;
                }

                goto backtrack_child_scc;
            } else if (succ_info.status == StateInfo::ONSTACK) {
                e.lowlink = std::min(e.lowlink, succ_info.ecd_stack_id);

                e.recurse = e.recurse || e.leaves_scc;
                e.remains_scc = true;
            } else {
            backtrack_child_scc:
                e.recurse = e.recurse || e.remains_scc;
                e.leaves_scc = true;
            }
        } while (e.next_successor() || e.next_action());

        return false;
    }

    bool push_ecd(StateID state_id, StateInfo& info)
    {
        assert(info.status == StateInfo::NEW);

        info.status = StateInfo::ONSTACK;
        StackInfo& scc_info = stack_[info.stack_id];

        if (scc_info.ec_transitions.empty()) {
            info.status = StateInfo::CLOSED;
            return false;
        }

        const auto stack_size = stack_ecd_.size();
        info.ecd_stack_id = stack_size;

        exploration_stack_ecd_.emplace_back(stack_size, scc_info);
        stack_ecd_.emplace_back(state_id);

        return true;
    }

    void scc_found_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer)
    {
        auto scc_begin = stack_ecd_.begin() + e.stackidx;
        auto scc_end = stack_ecd_.end();

        if (std::distance(scc_begin, scc_end) == 1) {
            const StateID scc_repr_id = *scc_begin;
            state_information_[scc_repr_id].status = StateInfo::CLOSED;
        } else if (e.recurse) {
            for (auto it = scc_begin; it != scc_end; ++it) {
                state_information_[*it].status = StateInfo::NEW;
            }

            const unsigned limit = exploration_stack_ecd_.size();

            for (unsigned i = e.stackidx; i < stack_ecd_.size(); ++i) {
                const StateID id = stack_ecd_[i];
                StateInfo& state_info = state_information_[id];

                if (state_info.status == StateInfo::NEW &&
                    push_ecd(id, state_info)) {
                    // Recursively run decomposition
                    find_and_decompose_sccs(limit, timer);
                }
            }

            assert(exploration_stack_ecd_.size() == limit);
        } else {
            // We found an end component, patch it
            const StateID scc_repr_id = *scc_begin;
            StateInfo& repr_state_info = state_information_[scc_repr_id];
            StackInfo& repr_stk = stack_[repr_state_info.stack_id];

            repr_state_info.status = StateInfo::CLOSED;

            // Spider construction
            for (auto it = scc_begin + 1; it != scc_end; ++it) {
                StateInfo& state_info = state_information_[*it];
                StackInfo& succ_stk = stack_[state_info.stack_id];

                state_info.status = StateInfo::CLOSED;

                // Move all non-EC transitions to representative state
                std::move(
                    succ_stk.non_ec_transitions.begin(),
                    succ_stk.non_ec_transitions.end(),
                    std::back_inserter(repr_stk.non_ec_transitions));

                // Free memory
                std::vector<QValueInfo>().swap(succ_stk.non_ec_transitions);

                set_min(repr_stk.conv_part, succ_stk.conv_part);
                succ_stk.conv_part = EngineValueType(INFINITE_VALUE);

                // Connect to representative state with zero cost action
                auto& t = succ_stk.non_ec_transitions.emplace_back(0.0_vt);
                t.scc_successors.emplace_back(scc_repr_id, 1.0_vt);
            }
        }

        // Previous iterators were invalidated during recursion!
        stack_ecd_.erase(stack_ecd_.begin() + e.stackidx, stack_ecd_.end());

        assert(stack_ecd_.size() == e.stackidx);
    }
};

} // namespace ta_topological_vi
} // namespace engines
} // namespace probfd

#endif // __TOPOLOGICAL_VALUE_ITERATION_H__