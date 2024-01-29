#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H
#error "This file should only be included from ta_topological_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/utils/iterators.h"

#include "probfd/cost_function.h"
#include "probfd/evaluator.h"
#include "probfd/progress_report.h"

#include "downward/utils/countdown_timer.h"

#include <type_traits>

namespace probfd::algorithms::ta_topological_vi {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Expanded state(s): " << expanded_states << std::endl;
    out << "  Terminal state(s): " << terminal_states << std::endl;
    out << "  Goal state(s): " << goal_states << std::endl;
    out << "  Pruned state(s): " << pruned << std::endl;
    out << "  Maximal SCCs: " << sccs << " (" << singleton_sccs
        << " are singleton)" << std::endl;
    out << "  Bellman backups: " << bellman_backups << std::endl;
}

template <typename State, typename Action, bool UseInterval>
auto TATopologicalValueIteration<State, Action, UseInterval>::StateInfo::
    get_status() const
{
    return explored ? (stack_id < UNDEF ? ONSTACK : CLOSED) : NEW;
}

template <typename State, typename Action, bool UseInterval>
auto TATopologicalValueIteration<State, Action, UseInterval>::StateInfo::
    get_ecd_status() const
{
    return explored ? (ecd_stack_id < UNDEF_ECD ? ONSTACK : CLOSED) : NEW;
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    ExplorationInfo(
        StateID state_id,
        StackInfo& stack_info,
        unsigned int stackidx)
    : state_id(state_id)
    , stack_info(stack_info)
    , stackidx(stackidx)
    , lowlink(stackidx)
{
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_transition(MDP& mdp)
{
    self_loop_prob = 0_vt;
    leaves_scc = false;

    aops.pop_back();
    transition.clear();

    return !aops.empty() &&
           forward_non_loop_transition(mdp, mdp.get_state(state_id));
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    forward_non_loop_transition(MDP& mdp, const State& state)
{
    do {
        mdp.generate_action_transitions(state, aops.back(), transition);
        successor = transition.begin();

        if (forward_non_loop_successor()) {
            const auto cost = mdp.get_action_cost(aops.back());
            non_zero = cost != 0.0_vt;
            if (non_zero) has_all_zero = false;
            stack_info.ec_transitions.emplace_back(cost);
            return true;
        }

        aops.pop_back();
        transition.clear();
        self_loop_prob = 0_vt;
    } while (!aops.empty());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    forward_non_loop_successor()
{
    do {
        if (successor->item != state_id) {
            return true;
        }

        self_loop_prob += successor->probability;
    } while (++successor != transition.end());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_successor()
{
    if (++successor != transition.end() && forward_non_loop_successor()) {
        return true;
    }

    auto& tinfo = stack_info.ec_transitions.back();

    const bool exits_only_solvable =
        tinfo.conv_part != AlgorithmValueType(INFINITE_VALUE);

    if (tinfo.finalize_transition(self_loop_prob)) {
        // Universally exiting -> Not part of scc
        // Update converged portion of q value and ignore this
        // transition
        set_min(stack_info.conv_part, tinfo.conv_part);
        stack_info.ec_transitions.pop_back();

        if (exits_only_solvable) {
            ++stack_info.active_exit_transitions;
            ++stack_info.active_transitions;
        }
    } else {
        if (non_zero || leaves_scc) {
            // Only some exiting or cost is non-zero ->
            // Not part of an end component
            // Move the transition to the set of non-EC transitions
            stack_info.non_ec_transitions.push_back(std::move(tinfo));
            stack_info.ec_transitions.pop_back();
        }

        if (exits_only_solvable) {
            if (leaves_scc) {
                ++stack_info.active_exit_transitions;
            }
            ++stack_info.active_transitions;
        }
        stack_info.transition_flags.emplace_back(
            exits_only_solvable && leaves_scc,
            exits_only_solvable);
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
ItemProbabilityPair<StateID>
TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    get_current_successor()
{
    return *successor;
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::QValueInfo::QValueInfo(
    value_t action_cost)
    : conv_part(action_cost)
{
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::QValueInfo::
    finalize_transition(value_t self_loop_prob)
{
    if (self_loop_prob != 0.0_vt) {
        // Apply self-loop normalization
        const value_t normalization = 1.0_vt / (1.0_vt - self_loop_prob);

        conv_part *= normalization;

        for (auto& pair : scc_successors) {
            pair.probability *= normalization;
        }
    }

    return scc_successors.empty();
}

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
auto TATopologicalValueIteration<State, Action, UseInterval>::QValueInfo::
    compute_q_value(ValueStore& value_store) const -> AlgorithmValueType
{
    AlgorithmValueType res = conv_part;

    for (auto& [state_id, prob] : scc_successors) {
        res += prob * value_store[state_id];
    }

    return res;
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::StackInfo::StackInfo(
    StateID state_id,
    AlgorithmValueType& value_ref)
    : state_id(state_id)
    , value(&value_ref)
{
}

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
bool TATopologicalValueIteration<State, Action, UseInterval>::StackInfo::
    update_value(ValueStore& value_store)
{
    AlgorithmValueType v = conv_part;

    for (const QValueInfo& info : non_ec_transitions) {
        set_min(v, info.compute_q_value(value_store));
    }

    if constexpr (UseInterval) {
        update(*value, v);
        return !value->bounds_equal();
    } else {
        return update(*value, v);
    }
}

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::ECDExplorationInfo::
    ECDExplorationInfo(StackInfo& stack_info, unsigned stackidx)
    : stack_info(stack_info)
    , stackidx(stackidx)
    , lowlink(stackidx)
{
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::
    ECDExplorationInfo::next_transition()
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

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::
    ECDExplorationInfo::next_successor()
{
    assert(!action->scc_successors.empty());
    return ++successor != action->scc_successors.end();
}

template <typename State, typename Action, bool UseInterval>
ItemProbabilityPair<StateID>
TATopologicalValueIteration<State, Action, UseInterval>::ECDExplorationInfo::
    get_current_successor()
{
    return *successor;
}

template <typename State, typename Action, bool UseInterval>
Interval TATopologicalValueIteration<State, Action, UseInterval>::solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    ProgressReport,
    double max_time)
{
    storage::PerStateStorage<AlgorithmValueType> value_store;
    return this
        ->solve(mdp, heuristic, mdp.get_state_id(state), value_store, max_time);
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
Statistics
TATopologicalValueIteration<State, Action, UseInterval>::get_statistics() const
{
    return statistics_;
}

template <typename State, typename Action, bool UseInterval>
Interval TATopologicalValueIteration<State, Action, UseInterval>::solve(
    MDP& mdp,
    const Evaluator& heuristic,
    StateID init_state_id,
    auto& value_store,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    push_state(
        init_state_id,
        state_information_[init_state_id],
        value_store[init_state_id]);

    for (;;) {
        ExplorationInfo* explore;

        do {
            explore = &exploration_stack_.back();
        } while (initialize_state(mdp, heuristic, *explore, value_store) &&
                 successor_loop(mdp, *explore, value_store, timer));

        // Iterative backtracking
        do {
            const unsigned stack_id = explore->stackidx;
            const unsigned lowlink = explore->lowlink;

            assert(stack_id >= lowlink);

            const bool backtrack_from_scc = stack_id == lowlink;

            // Check if an SCC was found.
            if (backtrack_from_scc) {
                scc_found(value_store, *explore, stack_id, timer);

                if (stack_id == 0) {
                    assert(stack_.empty());
                    assert(exploration_stack_.size() == 1);
                    exploration_stack_.pop_back();

                    if constexpr (UseInterval) {
                        return value_store[init_state_id];
                    } else {
                        return Interval(
                            value_store[init_state_id],
                            INFINITE_VALUE);
                    }
                }
            }

            assert(exploration_stack_.size() > 1);

            timer.throw_if_expired();

            const ExplorationInfo& successor = *explore--;

            const auto [succ_id, prob] = explore->get_current_successor();
            QValueInfo& tinfo = explore->stack_info.ec_transitions.back();

            if (backtrack_from_scc) {
                explore->recurse =
                    explore->recurse || !tinfo.scc_successors.empty();
                explore->leaves_scc = true;

                const AlgorithmValueType value = value_store[succ_id];
                tinfo.conv_part += prob * value;
                explore->exit_interval.lower =
                    std::min(explore->exit_interval.lower, value);
                explore->exit_interval.upper =
                    std::max(explore->exit_interval.upper, value);
            } else {
                explore->lowlink = std::min(explore->lowlink, lowlink);
                explore->exit_interval.lower = std::min(
                    explore->exit_interval.lower,
                    successor.exit_interval.lower);
                explore->exit_interval.upper = std::max(
                    explore->exit_interval.upper,
                    successor.exit_interval.upper);
                explore->has_all_zero =
                    explore->has_all_zero && successor.has_all_zero;
                explore->recurse = explore->recurse || successor.recurse ||
                                   explore->leaves_scc || explore->non_zero;

                tinfo.scc_successors.emplace_back(succ_id, prob);
                successor.stack_info.parents.emplace_back(
                    explore->stackidx,
                    explore->stack_info.transition_flags.size());
            }

            exploration_stack_.pop_back();
        } while (
            (!explore->next_successor() && !explore->next_transition(mdp)) ||
            !successor_loop(mdp, *explore, value_store, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::push_state(
    StateID state_id,
    StateInfo& state_info,
    AlgorithmValueType& value)
{
    const std::size_t stack_size = stack_.size();
    exploration_stack_.emplace_back(
        state_id,
        stack_.emplace_back(state_id, value),
        stack_size);
    state_info.explored = 1;
    state_info.stack_id = stack_size;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::successor_loop(
    MDP& mdp,
    ExplorationInfo& explore,
    auto& value_store,
    utils::CountdownTimer& timer)
{
    do {
        assert(!explore.stack_info.ec_transitions.empty());

        timer.throw_if_expired();

        const auto [succ_id, prob] = explore.get_current_successor();
        assert(succ_id != explore.state_id);

        StateInfo& succ_info = state_information_[succ_id];

        switch (succ_info.get_status()) {
        default: abort();
        case StateInfo::NEW: {
            push_state(succ_id, succ_info, value_store[succ_id]);
            return true; // recursion on new state
        }

        case StateInfo::CLOSED: {
            QValueInfo& tinfo = explore.stack_info.ec_transitions.back();

            explore.leaves_scc = true;
            explore.recurse = explore.recurse || !tinfo.scc_successors.empty();

            const AlgorithmValueType value = value_store[succ_id];
            tinfo.conv_part += prob * value;
            explore.exit_interval.lower =
                std::min(explore.exit_interval.lower, value);
            explore.exit_interval.upper =
                std::max(explore.exit_interval.upper, value);
            break;
        }

        case StateInfo::ONSTACK:
            unsigned succ_stack_id = succ_info.stack_id;
            explore.lowlink = std::min(explore.lowlink, succ_stack_id);
            explore.recurse =
                explore.recurse || explore.leaves_scc || explore.non_zero;

            QValueInfo& tinfo = explore.stack_info.ec_transitions.back();
            tinfo.scc_successors.emplace_back(succ_id, prob);

            auto& parents = stack_[succ_stack_id].parents;
            parents.emplace_back(
                explore.stackidx,
                explore.stack_info.transition_flags.size());
        }
    } while (explore.next_successor() || explore.next_transition(mdp));

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::initialize_state(
    MDP& mdp,
    const Evaluator& heuristic,
    ExplorationInfo& exp_info,
    auto& value_store)
{
    assert(
        state_information_[exp_info.state_id].get_status() ==
        StateInfo::ONSTACK);

    const State state = mdp.get_state(exp_info.state_id);

    const TerminationInfo state_term = mdp.get_termination_info(state);
    const value_t t_cost = state_term.get_cost();
    const value_t estimate = heuristic.evaluate(state);

    exp_info.stack_info.conv_part = AlgorithmValueType(t_cost);
    exp_info.exit_interval = Interval(t_cost);

    AlgorithmValueType& state_value = value_store[exp_info.state_id];

    if constexpr (UseInterval) {
        state_value.lower = estimate;
        state_value.upper = t_cost;
    } else {
        state_value = estimate;
    }

    if (t_cost != INFINITE_VALUE) {
        ++exp_info.stack_info.active_exit_transitions;
        ++exp_info.stack_info.active_transitions;
    }

    if (state_term.is_goal_state()) {
        ++statistics_.goal_states;
    } else if (estimate == t_cost) {
        ++statistics_.pruned;
        return false;
    }

    mdp.generate_applicable_actions(state, exp_info.aops);

    const size_t num_aops = exp_info.aops.size();

    exp_info.stack_info.non_ec_transitions.reserve(num_aops);
    exp_info.stack_info.ec_transitions.reserve(num_aops);

    ++statistics_.expanded_states;

    if (exp_info.aops.empty()) {
        ++statistics_.terminal_states;
    } else if (exp_info.forward_non_loop_transition(mdp, state)) {
        return true;
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::scc_found(
    auto& value_store,
    ExplorationInfo& exp_info,
    unsigned int stack_idx,
    utils::CountdownTimer& timer)
{
    using namespace std::views;

    auto scc = stack_ | drop(stack_idx);

    assert(!scc.empty());

    ++statistics_.sccs;

    if (exp_info.exit_interval.lower == INFINITE_VALUE ||
        (exp_info.exit_interval.lower == exp_info.exit_interval.upper &&
         exp_info.has_all_zero)) {
        for (StackInfo& stk_info : scc) {
            StateInfo& state_info = state_information_[stk_info.state_id];
            assert(state_info.get_status() == StateInfo::ONSTACK);
            update(*stk_info.value, exp_info.exit_interval.lower);
            state_info.stack_id = StateInfo::UNDEF;
        }

        stack_.erase(scc.begin(), scc.end());
        return;
    }

    if (scc.size() == 1) {
        // For singleton SCCs, we only have transitions which are
        // self-loops or go to a state that is topologically greater.
        // The state value is therefore the base value.
        StackInfo& single = scc.front();
        StateInfo& state_info = state_information_[single.state_id];
        assert(state_info.get_status() == StateInfo::ONSTACK);
        update(*single.value, single.conv_part);
        state_info.stack_id = StateInfo::UNDEF;
        ++statistics_.singleton_sccs;
        stack_.pop_back();
        return;
    }

    if (exp_info.recurse) {
        // Run recursive EC Decomposition
        scc_.reserve(scc.size());

        for (const auto state_id : scc | transform(&StackInfo::state_id)) {
            scc_.push_back(state_id);
        }

        // Run decomposition
        find_and_decompose_sccs(timer);

        for (StackInfo& stack_info : scc) {
            StateInfo& state_info = state_information_[stack_info.state_id];
            state_info.stack_id = StateInfo::UNDEF;
            assert(state_info.get_status() == StateInfo::CLOSED);
        }

        assert(exploration_stack_ecd_.empty());
    } else {
        // We found an end component, patch it
        StackInfo& repr_stk = scc.front();
        const StateID scc_repr_id = repr_stk.state_id;
        state_information_[scc_repr_id].stack_id = StateInfo::UNDEF;

        // Spider construction
        for (StackInfo& succ_stk : scc | drop(1)) {
            state_information_[succ_stk.state_id].stack_id = StateInfo::UNDEF;

            // Move all non-EC transitions to representative state
            std::move(
                succ_stk.non_ec_transitions.begin(),
                succ_stk.non_ec_transitions.end(),
                std::back_inserter(repr_stk.non_ec_transitions));

            // Free memory
            std::vector<QValueInfo>().swap(succ_stk.non_ec_transitions);

            set_min(repr_stk.conv_part, succ_stk.conv_part);

            succ_stk.conv_part = AlgorithmValueType(INFINITE_VALUE);

            // Connect to representative state with zero cost action
            auto& t = succ_stk.non_ec_transitions.emplace_back(0.0_vt);
            t.scc_successors.emplace_back(scc_repr_id, 1.0_vt);
        }
    }

    class Partition {
        std::vector<std::vector<int>::iterator> scc_index_to_local;
        std::vector<int> partition;
        std::vector<int>::iterator solvable_beg;
        std::vector<int>::iterator solvable_exits_beg;

    public:
        explicit Partition(std::size_t size)
            : scc_index_to_local(size)
            , partition(size, 0)
        {
            for (unsigned int i = 0; i != size; ++i) {
                scc_index_to_local[i] = partition.begin() + i;
                partition[i] = static_cast<int>(i);
            }

            solvable_beg = partition.begin();
            solvable_exits_beg = partition.begin();
        }

        auto solvable_begin() { return solvable_beg; }
        auto solvable_end() { return partition.end(); }

        auto solvable()
        {
            return std::ranges::subrange(solvable_begin(), solvable_end());
        }

        [[nodiscard]]
        bool has_solvable() const
        {
            return solvable_beg != partition.end();
        }

        void demote_unsolvable(int s)
        {
            auto local = scc_index_to_local[s];
            std::swap(scc_index_to_local[*solvable_beg], scc_index_to_local[s]);
            std::swap(*solvable_beg, *local);

            ++solvable_beg;
        }

        void demote_exit_unsolvable(int s)
        {
            auto local = scc_index_to_local[s];
            std::swap(
                scc_index_to_local[*solvable_exits_beg],
                scc_index_to_local[s]);
            std::swap(
                scc_index_to_local[*solvable_beg],
                scc_index_to_local[*solvable_exits_beg]);

            std::swap(*solvable_exits_beg, *local);
            std::swap(*solvable_beg, *solvable_exits_beg);

            ++solvable_beg;
            ++solvable_exits_beg;
        }

        void demote_exit_solvable(int s)
        {
            auto local = scc_index_to_local[s];
            std::swap(
                scc_index_to_local[*solvable_exits_beg],
                scc_index_to_local[s]);
            std::swap(*solvable_exits_beg, *local);

            ++solvable_exits_beg;
        }

        bool promote_solvable(int s)
        {
            if (!is_unsolvable(s)) {
                return false;
            }

            --solvable_beg;

            auto local = scc_index_to_local[s];
            std::swap(scc_index_to_local[*solvable_beg], scc_index_to_local[s]);
            std::swap(*solvable_beg, *local);

            return true;
        }

        void mark_non_exit_states_unsolvable()
        {
            solvable_beg = solvable_exits_beg;
        }

        bool is_unsolvable(int s)
        {
            return scc_index_to_local[s] < solvable_beg;
        }
    };

    // Set the value of unsolvable states of this SCC to -inf.
    // Start by partitioning states into inactive states, active exits and
    // active non-exists.
    // The partition is initialized optimistically, i.e., all states start out
    // as active exits.
    Partition partition(scc.size());

    for (std::size_t i = 0; i != scc.size(); ++i) {
        StackInfo& info = scc[i];

        assert(
            info.active_transitions != 0 || info.active_exit_transitions == 0);

        // Transform to local indices
        for (auto& parent_info : info.parents) {
            parent_info.parent_idx -= stack_idx;
        }

        if (info.active_exit_transitions == 0) {
            if (info.active_transitions > 0) {
                partition.demote_exit_solvable(i);
            } else {
                value_store[info.state_id] = AlgorithmValueType(INFINITE_VALUE);
                partition.demote_exit_unsolvable(i);
            }
        }
    }

    if (partition.has_solvable()) {
        // Compute the set of solvable states of this SCC.
        for (;;) {
            timer.throw_if_expired();

            // Collect states that can currently reach an exit and mark other
            // states unsolvable.
            auto unsolv_it = partition.solvable_begin();

            partition.mark_non_exit_states_unsolvable();

            for (auto it = partition.solvable_end();
                 it != partition.solvable_begin();) {
                for (const auto& [parent_idx, tr_idx] : scc[*--it].parents) {
                    StackInfo& pinfo = scc[parent_idx];

                    if (pinfo.transition_flags[tr_idx].is_active) {
                        partition.promote_solvable(parent_idx);
                    }
                }
            }

            // No new unsolvable states -> stop.
            if (unsolv_it == partition.solvable_begin()) break;

            // Run fixpoint iteration starting with the new unsolvable states
            // that could not reach an exit anymore.
            do {
                timer.throw_if_expired();

                StackInfo& scc_elem = scc[*unsolv_it];

                // The state was marked unsolvable.
                assert(partition.is_unsolvable(*unsolv_it));

                value_store[scc_elem.state_id] =
                    AlgorithmValueType(INFINITE_VALUE);

                for (const auto& [parent_idx, tr_idx] : scc_elem.parents) {
                    StackInfo& pinfo = scc[parent_idx];
                    auto& transition_flags = pinfo.transition_flags[tr_idx];

                    assert(
                        !transition_flags.is_active_exiting ||
                        transition_flags.is_active);

                    if (partition.is_unsolvable(parent_idx)) continue;

                    if (transition_flags.is_active_exiting) {
                        transition_flags.is_active_exiting = false;
                        transition_flags.is_active = false;

                        --pinfo.active_transitions;
                        --pinfo.active_exit_transitions;

                        if (pinfo.active_transitions == 0) {
                            partition.demote_exit_unsolvable(parent_idx);
                        } else if (pinfo.active_exit_transitions == 0) {
                            partition.demote_exit_solvable(parent_idx);
                        }
                    } else if (transition_flags.is_active) {
                        transition_flags.is_active = false;

                        --pinfo.active_transitions;

                        if (pinfo.active_transitions == 0) {
                            partition.demote_unsolvable(parent_idx);
                        }
                    }
                }
            } while (++unsolv_it != partition.solvable_begin());
        }
    }

    // Now run VI on the SCC until convergence
    bool converged;

    do {
        timer.throw_if_expired();

        converged = true;
        auto it = scc.begin();

        do {
            if (it->update_value(value_store)) converged = false;
            ++statistics_.bellman_backups;
        } while (++it != scc.end());
    } while (!converged);

    stack_.erase(scc.begin(), scc.end());
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::
    find_and_decompose_sccs(utils::CountdownTimer& timer)
{
    do {
        for (const StateID state_id : scc_) {
            state_information_[state_id].explored = 0;
        }

        for (const StateID state_id : scc_) {
            StateInfo& state_info = state_information_[state_id];
            if (state_info.get_ecd_status() != StateInfo::NEW) continue;

            state_info.explored = 1;
            state_info.ecd_stack_id = 0;
            exploration_stack_ecd_.emplace_back(stack_[state_info.stack_id], 0);
            stack_ecd_.emplace_back(state_id);

            for (;;) {
                ECDExplorationInfo* e;

                // DFS recursion
                do {
                    e = &exploration_stack_ecd_.back();
                } while (initialize_ecd(*e) && push_successor_ecd(*e, timer));

                // Iterative backtracking
                do {
                    const unsigned int stck = e->stackidx;
                    const unsigned int lowlink = e->lowlink;

                    assert(stck >= lowlink);

                    const bool backtracked_from_scc = stck == lowlink;

                    if (backtracked_from_scc) {
                        scc_found_ecd(*e);

                        if (stck == 0) {
                            assert(stack_ecd_.empty());
                            assert(exploration_stack_ecd_.size() == 1);
                            exploration_stack_ecd_.pop_back();
                            goto break_outer;
                        }
                    }

                    assert(exploration_stack_ecd_.size() > 1);

                    timer.throw_if_expired();

                    const ECDExplorationInfo& successor = *e--;

                    if (backtracked_from_scc) {
                        e->recurse = e->recurse || e->remains_scc;
                        e->leaves_scc = true;
                    } else {
                        e->lowlink = std::min(e->lowlink, lowlink);
                        e->recurse =
                            e->recurse || successor.recurse || e->leaves_scc;
                        e->remains_scc = true;
                    }

                    exploration_stack_ecd_.pop_back();
                } while ((!e->next_successor() && !e->next_transition()) ||
                         !push_successor_ecd(*e, timer));
            }

        break_outer:;
        }

        scc_.clear();
    } while (decomposition_queue_.pop_scc(scc_));

    assert(scc_.empty());
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::
    push_successor_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ_id = e.get_current_successor().item;
        StateInfo& succ_info = state_information_[succ_id];

        switch (succ_info.get_ecd_status()) {
        case StateInfo::NEW: {
            const auto stack_size = stack_ecd_.size();
            succ_info.explored = 1;
            succ_info.ecd_stack_id = stack_size;
            exploration_stack_ecd_.emplace_back(
                stack_[succ_info.stack_id],
                stack_size);
            stack_ecd_.emplace_back(succ_id);
            return true;
        }

        case StateInfo::CLOSED:
            e.recurse = e.recurse || e.remains_scc;
            e.leaves_scc = true;
            break;

        case StateInfo::ONSTACK:
            e.lowlink = std::min(e.lowlink, succ_info.ecd_stack_id);

            e.recurse = e.recurse || e.leaves_scc;
            e.remains_scc = true;
        }
    } while (e.next_successor() || e.next_transition());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::initialize_ecd(
    ECDExplorationInfo& exp_info)
{
    StackInfo& stack_info = exp_info.stack_info;

    if (stack_info.ec_transitions.empty()) {
        return false;
    }

    exp_info.action = stack_info.ec_transitions.begin();
    exp_info.end = stack_info.ec_transitions.end();
    exp_info.successor = exp_info.action->scc_successors.begin();

    return true;
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::scc_found_ecd(
    ECDExplorationInfo& e)
{
    namespace vws = std::views;

    auto scc = stack_ecd_ | std::views::drop(e.stackidx);

    if (scc.size() == 1) {
        state_information_[scc.front()].ecd_stack_id = StateInfo::UNDEF_ECD;
    } else if (e.recurse) {
        decomposition_queue_.register_new_scc();
        for (const StateID state_id : scc) {
            decomposition_queue_.add_scc_state(state_id);
            state_information_[state_id].ecd_stack_id = StateInfo::UNDEF_ECD;
        }
    } else {
        // We found an end component, patch it
        const StateID scc_repr_id = scc.front();
        StateInfo& repr_state_info = state_information_[scc_repr_id];
        StackInfo& repr_stk = stack_[repr_state_info.stack_id];

        repr_state_info.ecd_stack_id = StateInfo::UNDEF_ECD;

        // Spider construction
        for (const StateID state_id : scc | std::views::drop(1)) {
            StateInfo& state_info = state_information_[state_id];
            StackInfo& succ_stk = stack_[state_info.stack_id];

            state_info.ecd_stack_id = StateInfo::UNDEF_ECD;

            // Move all non-EC transitions to representative state
            std::move(
                succ_stk.non_ec_transitions.begin(),
                succ_stk.non_ec_transitions.end(),
                std::back_inserter(repr_stk.non_ec_transitions));

            // Free memory
            std::vector<QValueInfo>().swap(succ_stk.non_ec_transitions);

            set_min(repr_stk.conv_part, succ_stk.conv_part);
            succ_stk.conv_part = AlgorithmValueType(INFINITE_VALUE);

            // Connect to representative state with zero cost action
            auto& t = succ_stk.non_ec_transitions.emplace_back(0.0_vt);
            t.scc_successors.emplace_back(scc_repr_id, 1.0_vt);
        }
    }

    stack_ecd_.erase(scc.begin(), scc.end());

    assert(stack_ecd_.size() == e.stackidx);
}

} // namespace probfd::algorithms::ta_topological_vi
