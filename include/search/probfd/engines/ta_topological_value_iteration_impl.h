#ifndef GUARD_INCLUDE_PROBFD_ENGINES_TA_TOPOLOGICAL_VALUE_ITERATION_H
#error "This file should only be included from ta_topological_value_iteration.h"
#endif

#include "probfd/engines/utils.h"

#include "probfd/utils/iterators.h"

#include "downward/utils/countdown_timer.h"

#include <type_traits>

namespace probfd {
namespace engines {
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
    ExplorationInfo(
        std::vector<Action> aops,
        Distribution<StateID> transition,
        unsigned stackidx)
    : aops(std::move(aops))
    , transition(std::move(transition))
    , successor(this->transition.begin())
    , stackidx(stackidx)
    , lowlink(stackidx)
{
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_transition(MDP& mdp, StateID state_id)
{
    for (aops.pop_back(); !aops.empty(); aops.pop_back()) {
        transition.clear();
        mdp.generate_action_transitions(state_id, aops.back(), transition);

        if (!transition.is_dirac(state_id)) {
            successor = transition.begin();
            return true;
        }
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_successor()
{
    return ++successor != transition.end();
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
    compute_q_value(ValueStore& value_store) const -> EngineValueType
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

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::StackInfo::StackInfo(
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

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
bool TATopologicalValueIteration<State, Action, UseInterval>::StackInfo::
    update_value(ValueStore& value_store)
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

template <typename State, typename Action, bool UseInterval>
TATopologicalValueIteration<State, Action, UseInterval>::ECDExplorationInfo::
    ECDExplorationInfo(StackInfo& stack_info, unsigned stackidx)
    : action(stack_info.ec_transitions.begin())
    , end(stack_info.ec_transitions.end())
    , successor(action->scc_successors.begin())
    , stackidx(stackidx)
    , lowlink(stackidx)
    , stack_info(stack_info)
{
    assert(!stack_info.ec_transitions.empty());
    assert(!action->scc_successors.empty());
    assert(action < end);
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
    double max_time)
{
    storage::PerStateStorage<EngineValueType> value_store;
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
template <typename ValueStore>
Interval TATopologicalValueIteration<State, Action, UseInterval>::solve(
    MDP& mdp,
    Evaluator& heuristic,
    StateID init_state_id,
    ValueStore& value_store,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    StateInfo& iinfo = state_information_[init_state_id];
    EngineValueType& init_value = value_store[init_state_id];

    if (!push_state(mdp, heuristic, init_state_id, iinfo, init_value)) {
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
            mdp,
            heuristic,
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
                explore->recurse =
                    explore->recurse || recurse || explore->nz_or_leaves_scc;

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
            if (tinfo.finalize_transition()) {
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
            if (explore->next_transition(mdp, state_id)) {
                const auto cost =
                    mdp.get_action_cost(explore->get_current_action());
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

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
bool TATopologicalValueIteration<State, Action, UseInterval>::successor_loop(
    MDP& mdp,
    Evaluator& heuristic,
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
                if (push_state(mdp, heuristic, succ_id, succ_info, s_value)) {
                    return true; // recursion on new state
                }

                [[fallthrough]];
            case StateInfo::CLOSED:
                explore.nz_or_leaves_scc = true;
                explore.recurse =
                    explore.recurse || !tinfo.scc_successors.empty();

                tinfo.conv_part += prob * s_value;
                break;

            case StateInfo::ONSTACK:
                explore.lowlink = std::min(explore.lowlink, succ_info.stack_id);
                explore.recurse = explore.recurse || explore.nz_or_leaves_scc;

                tinfo.scc_successors.emplace_back(succ_id, prob);
            }
        } while (explore.next_successor());

        if (tinfo.finalize_transition()) {
            set_min(stack_info.conv_part, tinfo.conv_part);
            stack_info.ec_transitions.pop_back();
        } else if (explore.nz_or_leaves_scc) {
            stack_info.non_ec_transitions.push_back(std::move(tinfo));
            stack_info.ec_transitions.pop_back();
        }

        if (!explore.next_transition(mdp, state_id)) {
            return false;
        }

        const auto cost = mdp.get_action_cost(explore.get_current_action());

        explore.nz_or_leaves_scc = cost != 0.0_vt;
        stack_info.ec_transitions.emplace_back(cost);
    }
}

template <typename State, typename Action, bool UseInterval>
bool TATopologicalValueIteration<State, Action, UseInterval>::push_state(
    MDP& mdp,
    Evaluator& heuristic,
    StateID state_id,
    StateInfo& state_info,
    EngineValueType& state_value)
{
    assert(state_info.status == StateInfo::NEW);

    const State state = mdp.get_state(state_id);

    const TerminationInfo state_term = mdp.get_termination_info(state);
    const auto t_cost = state_term.get_cost();

    if (state_term.is_goal_state()) {
        ++statistics_.goal_states;
    }

    const EvaluationResult h_eval = heuristic.evaluate(state);
    const auto estimate = h_eval.get_estimate();

    if (h_eval.is_unsolvable()) {
        ++statistics_.pruned;
        state_value = EngineValueType(estimate);
        state_info.status = StateInfo::CLOSED;
        return false;
    }

    state_info.status = StateInfo::ONSTACK;

    std::vector<Action> aops;
    mdp.generate_applicable_actions(state_id, aops);
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
        mdp.generate_action_transitions(state_id, aops.back(), transition);

        assert(!transition.empty());

        // Check for self loop
        if (!transition.is_dirac(state_id)) {
            std::size_t stack_size = stack_.size();

            state_info.stack_id = stack_size;

            // Found non self loop action, push and return success.
            const auto cost = mdp.get_action_cost(aops.back());

            ExplorationInfo& explore = exploration_stack_.emplace_back(
                std::move(aops),
                std::move(transition),
                stack_size);

            explore.nz_or_leaves_scc = cost != 0.0_vt;

            auto& s_info =
                stack_.emplace_back(state_id, state_value, t_cost, aops.size());

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

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore, typename StackIterator>
void TATopologicalValueIteration<State, Action, UseInterval>::scc_found(
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

template <typename State, typename Action, bool UseInterval>
void TATopologicalValueIteration<State, Action, UseInterval>::
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
        } while (!e->next_successor() && !e->next_transition());
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
        case StateInfo::NEW:
            if (push_ecd(succ_id, succ_info)) {
                return true;
            }

            [[fallthrough]];
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
bool TATopologicalValueIteration<State, Action, UseInterval>::push_ecd(
    StateID state_id,
    StateInfo& info)
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

    exploration_stack_ecd_.emplace_back(scc_info, stack_size);
    stack_ecd_.emplace_back(state_id);

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

} // namespace ta_topological_vi
} // namespace engines
} // namespace probfd