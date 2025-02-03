#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_TOPOLOGICAL_VALUE_ITERATION_H
#error "This file should only be included from topological_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "probfd/heuristic.h"
#include "probfd/progress_report.h"

#include "downward/utils/countdown_timer.h"

#include <type_traits>

namespace probfd::algorithms::topological_vi {

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
TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    ExplorationInfo(StateID state_id, StackInfo& stack_info, unsigned stackidx)
    : state_id(state_id)
    , stack_info(stack_info)
    , stackidx(stackidx)
    , lowlink(stackidx)
{
}

template <typename State, typename Action, bool UseInterval>
void TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    update_lowlink(unsigned upd)
{
    lowlink = std::min(lowlink, upd);
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_transition(MDPType& mdp)
{
    aops.pop_back();
    transition.clear();

    self_loop_prob = 0_vt;

    return !aops.empty() &&
           forward_non_loop_transition(mdp, mdp.get_state(state_id));
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_successor()
{
    ++successor;
    if (forward_non_loop_successor()) return true;

    auto& tinfo = stack_info.nconv_qs.back();

    if (tinfo.finalize_transition(self_loop_prob)) {
        if (set_min(stack_info.conv_part, tinfo.conv_part)) {
            stack_info.best_converged = tinfo.action;
        }
        stack_info.nconv_qs.pop_back();
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    forward_non_loop_transition(MDPType& mdp, const State& state)
{
    do {
        mdp.generate_action_transitions(state, aops.back(), transition);
        successor = transition.begin();

        if (forward_non_loop_successor()) {
            stack_info.nconv_qs.emplace_back(
                aops.back(),
                mdp.get_action_cost(aops.back()));
            return true;
        }

        aops.pop_back();
        transition.clear();
    } while (!aops.empty());

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
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
Action& TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    get_current_action()
{
    return aops.back();
}

template <typename State, typename Action, bool UseInterval>
ItemProbabilityPair<StateID>
TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    get_current_successor()
{
    return *successor;
}

template <typename State, typename Action, bool UseInterval>
TopologicalValueIteration<State, Action, UseInterval>::QValueInfo::QValueInfo(
    Action action,
    value_t action_cost)
    : action(action)
    , conv_part(action_cost)
{
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::QValueInfo::
    finalize_transition(value_t self_loop_prob)
{
    if (self_loop_prob != 0_vt) {
        // Apply self-loop normalization
        const value_t normalization = 1_vt / (1_vt - self_loop_prob);

        conv_part *= normalization;

        for (auto& pair : nconv_successors) {
            pair.probability *= normalization;
        }
    }

    return nconv_successors.empty();
}

template <typename State, typename Action, bool UseInterval>
auto TopologicalValueIteration<State, Action, UseInterval>::QValueInfo::
    compute_q_value() const -> AlgorithmValueType
{
    AlgorithmValueType res = conv_part;

    for (auto& [value, prob] : nconv_successors) {
        res += prob * (*value);
    }

    return res;
}

template <typename State, typename Action, bool UseInterval>
TopologicalValueIteration<State, Action, UseInterval>::StackInfo::StackInfo(
    StateID state_id,
    AlgorithmValueType& value_ref)
    : state_id(state_id)
    , value(&value_ref)
{
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::StackInfo::
    update_value(value_t convergence_epsilon)
{
    AlgorithmValueType v = conv_part;
    best_action = best_converged;

    for (const QValueInfo& info : nconv_qs) {
        if (set_min(v, info.compute_q_value())) {
            best_action = info.action;
        }
    }

    if constexpr (UseInterval) {
        update(*value, v, convergence_epsilon);
        return !value->bounds_approximately_equal(convergence_epsilon);
    } else {
        return update(*value, v, convergence_epsilon);
    }
}

template <typename State, typename Action, bool UseInterval>
TopologicalValueIteration<State, Action, UseInterval>::
    TopologicalValueIteration(value_t epsilon, bool expand_goals)
    : IterativeMDPAlgorithm<State, Action>(epsilon)
    , expand_goals_(expand_goals)
{
}

template <typename State, typename Action, bool UseInterval>
auto TopologicalValueIteration<State, Action, UseInterval>::compute_policy(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> state,
    ProgressReport,
    double max_time) -> std::unique_ptr<PolicyType>
{
    storage::PerStateStorage<AlgorithmValueType> value_store;
    std::unique_ptr<MapPolicy> policy(new MapPolicy(&mdp));
    this->solve(
        mdp,
        heuristic,
        mdp.get_state_id(state),
        value_store,
        max_time,
        policy.get());
    return policy;
}

template <typename State, typename Action, bool UseInterval>
Interval TopologicalValueIteration<State, Action, UseInterval>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> state,
    ProgressReport,
    double max_time)
{
    storage::PerStateStorage<AlgorithmValueType> value_store;
    return this
        ->solve(mdp, heuristic, mdp.get_state_id(state), value_store, max_time);
}

template <typename State, typename Action, bool UseInterval>
void TopologicalValueIteration<State, Action, UseInterval>::print_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
}

template <typename State, typename Action, bool UseInterval>
Statistics
TopologicalValueIteration<State, Action, UseInterval>::get_statistics() const
{
    return statistics_;
}

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
Interval TopologicalValueIteration<State, Action, UseInterval>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    StateID init_state_id,
    ValueStore& value_store,
    double max_time,
    MapPolicy* policy)
{
    utils::CountdownTimer timer(max_time);

    StateInfo& iinfo = state_information_[init_state_id];
    AlgorithmValueType& init_value = value_store[init_state_id];

    push_state(init_state_id, iinfo, init_value);

    for (;;) {
        ExplorationInfo* explore;

        do {
            explore = &exploration_stack_.back();
        } while (initialize_state(mdp, heuristic, *explore, value_store) &&
                 successor_loop(mdp, *explore, value_store, timer));

        do {
            // Check if an SCC was found.
            const unsigned stack_id = explore->stackidx;
            const unsigned lowlink = explore->lowlink;
            const bool backtrack_from_scc = stack_id == lowlink;

            if (backtrack_from_scc) {
                scc_found(stack_ | std::views::drop(stack_id), policy, timer);
            }

            exploration_stack_.pop_back();

            if (exploration_stack_.empty()) {
                if constexpr (UseInterval) {
                    return init_value;
                } else {
                    return Interval(init_value, INFINITE_VALUE);
                }
            }

            timer.throw_if_expired();

            explore = &exploration_stack_.back();

            const auto [succ_id, prob] = explore->get_current_successor();
            AlgorithmValueType& s_value = value_store[succ_id];
            QValueInfo& tinfo = explore->stack_info.nconv_qs.back();

            if (backtrack_from_scc) {
                tinfo.conv_part += prob * s_value;
            } else {
                explore->update_lowlink(lowlink);
                tinfo.nconv_successors.emplace_back(&s_value, prob);
            }
        } while (
            (!explore->next_successor() && !explore->next_transition(mdp)) ||
            !successor_loop(mdp, *explore, value_store, timer));
    }
}

template <typename State, typename Action, bool UseInterval>
void TopologicalValueIteration<State, Action, UseInterval>::push_state(
    StateID state_id,
    StateInfo& state_info,
    AlgorithmValueType& state_value)
{
    const std::size_t stack_size = stack_.size();
    exploration_stack_.emplace_back(
        state_id,
        stack_.emplace_back(state_id, state_value),
        stack_size);
    state_info.stack_id = stack_size;
    state_info.status = StateInfo::ONSTACK;
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::initialize_state(
    MDPType& mdp,
    EvaluatorType& heuristic,
    ExplorationInfo& exp_info,
    auto& value_store)
{
    assert(state_information_[exp_info.state_id].status == StateInfo::NEW);

    const State state = mdp.get_state(exp_info.state_id);

    const TerminationInfo state_eval = mdp.get_termination_info(state);
    const value_t t_cost = state_eval.get_cost();
    const value_t estimate = heuristic.evaluate(state);

    exp_info.stack_info.conv_part = AlgorithmValueType(t_cost);

    AlgorithmValueType& state_value = value_store[exp_info.state_id];

    if constexpr (UseInterval) {
        state_value.lower = estimate;
        state_value.upper = t_cost;
    } else {
        state_value = estimate;
    }

    if (state_eval.is_goal_state()) {
        ++statistics_.goal_states;

        if (!expand_goals_) {
            ++statistics_.pruned;
            return false;
        }
    } else if (estimate == t_cost) {
        ++statistics_.pruned;
        return false;
    }

    mdp.generate_applicable_actions(state, exp_info.aops);

    const size_t num_aops = exp_info.aops.size();

    exp_info.stack_info.nconv_qs.reserve(num_aops);

    ++statistics_.expanded_states;

    if (exp_info.aops.empty()) {
        ++statistics_.terminal_states;
    } else if (exp_info.forward_non_loop_transition(mdp, state)) {
        return true;
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
bool TopologicalValueIteration<State, Action, UseInterval>::successor_loop(
    MDPType& mdp,
    ExplorationInfo& explore,
    ValueStore& value_store,
    utils::CountdownTimer& timer)
{
    do {
        assert(!explore.stack_info.nconv_qs.empty());
        QValueInfo& tinfo = explore.stack_info.nconv_qs.back();

        do {
            timer.throw_if_expired();

            const auto [succ_id, prob] = explore.get_current_successor();
            assert(succ_id != explore.state_id);
            StateInfo& succ_info = state_information_[succ_id];
            AlgorithmValueType& s_value = value_store[succ_id];

            switch (succ_info.status) {
            default: abort();
            case StateInfo::NEW: {
                push_state(succ_id, succ_info, s_value);
                return true; // recursion on new state
            }

            case StateInfo::CLOSED: tinfo.conv_part += prob * s_value; break;

            case StateInfo::ONSTACK:
                explore.update_lowlink(succ_info.stack_id);
                tinfo.nconv_successors.emplace_back(&s_value, prob);
            }
        } while (explore.next_successor());
    } while (explore.next_transition(mdp));

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TopologicalValueIteration<State, Action, UseInterval>::scc_found(
    auto scc,
    MapPolicy* policy,
    utils::CountdownTimer& timer)
{
    assert(!scc.empty());

    ++statistics_.sccs;

    if (scc.size() == 1) {
        // Singleton SCCs can only transition to a child SCC. The state
        // value has already converged due to topological ordering.
        ++statistics_.singleton_sccs;
        StackInfo& single = scc.front();
        StateInfo& state_info = state_information_[single.state_id];
        *single.value = single.conv_part;
        assert(state_info.status == StateInfo::ONSTACK);
        state_info.status = StateInfo::CLOSED;
    } else {
        // Mark all states as closed
        for (StackInfo& stk_info : scc) {
            StateInfo& state_info = state_information_[stk_info.state_id];
            assert(state_info.status == StateInfo::ONSTACK);
            assert(!stk_info.nconv_qs.empty());
            state_info.status = StateInfo::CLOSED;
        }

        // Now run VI on the SCC until convergence
        bool converged;

        do {
            timer.throw_if_expired();

            converged = true;
            auto it = scc.begin();

            do {
                if (it->update_value(this->epsilon)) converged = false;
                ++statistics_.bellman_backups;
            } while (++it != scc.end());
        } while (!converged);

        // Extract a policy from this SCC
        if (policy) {
            for (StackInfo& stk_info : scc) {
                if constexpr (UseInterval) {
                    policy->emplace_decision(
                        stk_info.state_id,
                        *stk_info.best_action,
                        *stk_info.value);
                } else {
                    policy->emplace_decision(
                        stk_info.state_id,
                        *stk_info.best_action,
                        Interval(*stk_info.value, INFINITE_VALUE));
                }
            }
        }
    }

    stack_.erase(scc.begin(), scc.end());
}

} // namespace probfd::algorithms::topological_vi
