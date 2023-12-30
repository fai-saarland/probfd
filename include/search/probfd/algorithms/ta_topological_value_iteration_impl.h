#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H
#error "This file should only be included from ta_topological_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/utils/iterators.h"

#include "probfd/cost_function.h"

#include "downward/utils/countdown_timer.h"

#include <type_traits>

namespace probfd {
namespace algorithms {
namespace ta_topological_vi {

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
TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    ExplorationInfo(unsigned int stackidx)
    : stackidx(stackidx)
    , lowlink(stackidx)
{
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_transition(MDP& mdp, StackInfo& stack_info, StateID state_id)
{
    aops.pop_back();
    transition.clear();

    return !aops.empty() && forward_non_loop_transition(
                                mdp,
                                stack_info,
                                mdp.get_state(state_id),
                                state_id);
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    forward_non_loop_transition(
        MDP& mdp,
        StackInfo& stack_info,
        const State& state,
        StateID state_id)
{
    do {
        mdp.generate_action_transitions(state, aops.back(), transition);

        if (!transition.is_dirac(state_id)) {
            successor = transition.begin();
            const auto cost = mdp.get_action_cost(aops.back());
            nz_or_leaves_scc = cost != 0.0_vt;
            stack_info.ec_transitions.emplace_back(cost);
            return true;
        }

        aops.pop_back();
        transition.clear();
    } while (!aops.empty());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_successor(StackInfo& stack_info)
{
    if (++successor != transition.end()) {
        return true;
    }

    auto& tinfo = stack_info.ec_transitions.back();

    if (tinfo.finalize_transition()) {
        // Universally exiting -> Not part of scc
        // Update converged portion of q value and ignore this
        // transition
        set_min(stack_info.conv_part, tinfo.conv_part);
        stack_info.ec_transitions.pop_back();
    } else if (nz_or_leaves_scc) {
        // Only some exiting or cost is non-zero ->
        // Not part of an end component
        // Move the transition to the set of non-EC transitions
        stack_info.non_ec_transitions.push_back(std::move(tinfo));
        stack_info.ec_transitions.pop_back();
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
Action& TATopologicalValueIteration<State, Action, UseInterval>::
    ExplorationInfo::get_current_action()
{
    return aops.back();
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
    finalize_transition()
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

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
auto TATopologicalValueIteration<State, Action, UseInterval>::QValueInfo::
    compute_q_value(ValueStore& value_store) const -> AlgorithmValueType
{
    AlgorithmValueType res = conv_part;

    for (auto& [state_id, prob] : scc_successors) {
        res += prob * value_store[state_id];
    }

    if (self_loop_prob != 0.0_vt) {
        res *= self_loop_prob;
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
    : stackidx(stackidx)
    , lowlink(stackidx)
    , stack_info(stack_info)
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
auto TATopologicalValueIteration<State, Action, UseInterval>::
    ECDExplorationInfo::get_current_action() -> QValueInfo&
{
    return *action;
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
    Evaluator& heuristic,
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
        StackInfo* stack_info;
        StateID state_id;

        do {
            explore = &exploration_stack_.back();
            stack_info = &stack_[explore->stackidx];
            state_id = stack_info->state_id;
        } while (initialize_state(
                     mdp,
                     heuristic,
                     *explore,
                     *stack_info,
                     state_id,
                     value_store[state_id]) &&
                 successor_loop(
                     mdp,
                     *explore,
                     *stack_info,
                     state_id,
                     value_store,
                     timer));

        // Iterative backtracking
        do {
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
                if constexpr (UseInterval) {
                    return value_store[init_state_id];
                } else {
                    return Interval(value_store[init_state_id], INFINITE_VALUE);
                }
            }

            timer.throw_if_expired();

            explore = &exploration_stack_.back();
            stack_info = &stack_[explore->stackidx];
            state_id = stack_info->state_id;

            const auto [succ_id, prob] = explore->get_current_successor();

            QValueInfo& tinfo = stack_info->ec_transitions.back();

            if (onstack) {
                explore->lowlink = std::min(explore->lowlink, lowlink);
                explore->recurse =
                    explore->recurse || recurse || explore->nz_or_leaves_scc;

                tinfo.scc_successors.emplace_back(succ_id, prob);
            } else {
                const AlgorithmValueType& s_value = value_store[succ_id];
                explore->recurse =
                    explore->recurse || !tinfo.scc_successors.empty();
                explore->nz_or_leaves_scc = true;

                tinfo.conv_part += prob * s_value;
            }
        } while ((!explore->next_successor(*stack_info) &&
                  !explore->next_transition(mdp, *stack_info, state_id)) ||
                 !successor_loop(
                     mdp,
                     *explore,
                     *stack_info,
                     state_id,
                     value_store,
                     timer));
    }
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::push_state(
    StateID state_id,
    StateInfo& state_info,
    AlgorithmValueType& value)
{
    const std::size_t stack_size = stack_.size();
    exploration_stack_.emplace_back(stack_size);
    stack_.emplace_back(state_id, value);
    state_info.stack_id = stack_size;
    state_info.status = StateInfo::ONSTACK;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::successor_loop(
    MDP& mdp,
    ExplorationInfo& explore,
    StackInfo& stack_info,
    StateID state_id,
    auto& value_store,
    utils::CountdownTimer& timer)
{
    do {
        assert(!stack_info.ec_transitions.empty());

        QValueInfo& tinfo = stack_info.ec_transitions.back();
        timer.throw_if_expired();

        const auto [succ_id, prob] = explore.get_current_successor();

        if (succ_id == state_id) {
            tinfo.self_loop_prob += prob;
            continue;
        }

        StateInfo& succ_info = state_information_[succ_id];
        AlgorithmValueType& s_value = value_store[succ_id];
        int status = succ_info.status;

        switch (status) {
        case StateInfo::NEW: {
            push_state(succ_id, succ_info, s_value);
            return true; // recursion on new state
        }

        case StateInfo::CLOSED:
            explore.nz_or_leaves_scc = true;
            explore.recurse = explore.recurse || !tinfo.scc_successors.empty();

            tinfo.conv_part += prob * s_value;
            break;

        case StateInfo::ONSTACK:
            explore.lowlink = std::min(explore.lowlink, succ_info.stack_id);
            explore.recurse = explore.recurse || explore.nz_or_leaves_scc;

            tinfo.scc_successors.emplace_back(succ_id, prob);
        }
    } while (explore.next_successor(stack_info) ||
             explore.next_transition(mdp, stack_info, state_id));

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::initialize_state(
    MDP& mdp,
    Evaluator& heuristic,
    ExplorationInfo& exp_info,
    StackInfo& stack_info,
    StateID state_id,
    AlgorithmValueType& state_value)
{
    StateInfo& state_info = state_information_[state_id];

    assert(state_info.status == StateInfo::ONSTACK);

    const State state = mdp.get_state(state_id);

    const TerminationInfo state_term = mdp.get_termination_info(state);
    const value_t t_cost = state_term.get_cost();
    const value_t estimate = heuristic.evaluate(state);

    stack_info.conv_part = t_cost;

    if constexpr (UseInterval) {
        state_value.lower = estimate;
        state_value.upper = t_cost;
    } else {
        state_value = estimate;
    }

    if (state_term.is_goal_state()) {
        ++statistics_.goal_states;
    } else if (estimate == t_cost) {
        ++statistics_.pruned;
        return false;
    }

    mdp.generate_applicable_actions(state, exp_info.aops);

    const size_t num_aops = exp_info.aops.size();

    stack_info.non_ec_transitions.reserve(num_aops);
    stack_info.ec_transitions.reserve(num_aops);

    ++statistics_.expanded_states;

    if (exp_info.aops.empty()) {
        ++statistics_.terminal_states;
    } else if (exp_info.forward_non_loop_transition(
                   mdp,
                   stack_info,
                   state,
                   state_id)) {
        return true;
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::scc_found(
    auto& value_store,
    ExplorationInfo& exp_info,
    auto begin,
    auto end,
    utils::CountdownTimer& timer)
{
    assert(begin != end);

    ++statistics_.sccs;

    if (std::distance(begin, end) == 1) {
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
            StateInfo& state_info = state_information_[it->state_id];
            assert(state_info.status == StateInfo::ONSTACK);
            state_info.status = StateInfo::NEW;
        }

        for (auto it = begin; it != end; ++it) {
            const StateID id = it->state_id;
            StateInfo& state_info = state_information_[id];

            if (state_info.status == StateInfo::NEW) {
                // Run decomposition
                find_and_decompose_sccs(id, state_info, timer);
            }

            assert(state_info.status == StateInfo::CLOSED);
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

            succ_stk.conv_part = AlgorithmValueType(INFINITE_VALUE);

            // Connect to representative state with zero cost action
            auto& t = succ_stk.non_ec_transitions.emplace_back(0.0_vt);
            t.scc_successors.emplace_back(scc_repr_id, 1.0_vt);
        }
    }

    // Now run VI on the SCC until convergence
    bool converged;

    do {
        timer.throw_if_expired();

        converged = true;
        auto it = begin;

        do {
            if (it->update_value(value_store)) converged = false;
            ++statistics_.bellman_backups;
        } while (++it != end);
    } while (!converged);

    stack_.erase(begin, end);
}

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::
    find_and_decompose_sccs(
        StateID state_id,
        StateInfo& state_info,
        utils::CountdownTimer& timer)
{
    const unsigned limit = exploration_stack_ecd_.size();

    const auto stack_size = stack_ecd_.size();
    state_info.ecd_stack_id = stack_size;
    state_info.status = StateInfo::ONSTACK;
    exploration_stack_ecd_.emplace_back(
        stack_[state_info.stack_id],
        stack_size);
    stack_ecd_.emplace_back(state_id);

    for (;;) {
        ECDExplorationInfo* e;

        // DFS recursion
        do {
            e = &exploration_stack_ecd_.back();
        } while (initialize_ecd(*e) && push_successor_ecd(*e, timer));

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

            if (exploration_stack_ecd_.size() == limit) {
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
        } while ((!e->next_successor() && !e->next_transition()) ||
                 !push_successor_ecd(*e, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::
    push_successor_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer)
{
    do {
        timer.throw_if_expired();

        const StateID succ_id = e.get_current_successor().item;
        StateInfo& succ_info = state_information_[succ_id];

        switch (succ_info.status) {
        case StateInfo::NEW: {
            const auto stack_size = stack_ecd_.size();
            succ_info.ecd_stack_id = stack_size;
            succ_info.status = StateInfo::ONSTACK;
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
    ECDExplorationInfo& e,
    utils::CountdownTimer& timer)
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

        for (unsigned i = e.stackidx; i < stack_ecd_.size(); ++i) {
            const StateID id = stack_ecd_[i];
            StateInfo& state_info = state_information_[id];

            if (state_info.status == StateInfo::NEW) {
                // Recursively run decomposition
                find_and_decompose_sccs(id, state_info, timer);
            }

            assert(state_info.status == StateInfo::CLOSED);
        }
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
            succ_stk.conv_part = AlgorithmValueType(INFINITE_VALUE);

            // Connect to representative state with zero cost action
            auto& t = succ_stk.non_ec_transitions.emplace_back(0.0_vt);
            t.scc_successors.emplace_back(scc_repr_id, 1.0_vt);
        }
    }

    // Previous iterators were invalidated during recursion!
    stack_ecd_.erase(stack_ecd_.begin() + e.stackidx, stack_ecd_.end());

    assert(stack_ecd_.size() == e.stackidx);
}

} // namespace ta_topological_vi
} // namespace algorithms
} // namespace probfd