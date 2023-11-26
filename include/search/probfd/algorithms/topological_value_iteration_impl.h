#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_TOPOLOGICAL_VALUE_ITERATION_H
#error "This file should only be included from topological_value_iteration.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/policies/map_policy.h"

#include "downward/utils/countdown_timer.h"

#include <type_traits>

namespace probfd {
namespace algorithms {
namespace topological_vi {

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
    ExplorationInfo(
        std::vector<Action> aops,
        Distribution<StateID> transition,
        StateID state_id,
        unsigned stackidx)
    : aops(std::move(aops))
    , transition(std::move(transition))
    , successor(this->transition.begin())
    , state_id(state_id)
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
    next_transition(MDP& mdp, StackInfo& stack_info, StateID state_id)
{
    for (aops.pop_back(); !aops.empty(); aops.pop_back()) {
        transition.clear();
        const State state = mdp.get_state(state_id);
        mdp.generate_action_transitions(state, aops.back(), transition);

        if (!transition.is_dirac(state_id)) {
            successor = transition.begin();
            stack_info.nconv_qs.emplace_back(
                aops.back(),
                mdp.get_action_cost(aops.back()));
            return true;
        }
    }

    return false;
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::ExplorationInfo::
    next_successor()
{
    return ++successor != transition.end();
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
    finalize_transition()
{
    if (self_loop_prob != 0_vt) {
        // Calculate self-loop normalization factor
        self_loop_prob = 1_vt / (1_vt - self_loop_prob);

        if (nconv_successors.empty()) {
            // Apply self-loop normalization immediately
            conv_part *= self_loop_prob;
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

    if (self_loop_prob != 0_vt) {
        res *= self_loop_prob;
    }

    return res;
}

template <typename State, typename Action, bool UseInterval>
TopologicalValueIteration<State, Action, UseInterval>::StackInfo::StackInfo(
    StateID state_id,
    AlgorithmValueType& value_ref,
    value_t terminal_cost,
    unsigned num_aops)
    : state_id(state_id)
    , value(&value_ref)
    , terminal_cost(terminal_cost)
    , conv_part(terminal_cost)
{
    nconv_qs.reserve(num_aops);
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::StackInfo::
    update_value()
{
    AlgorithmValueType v = conv_part;
    best_action = best_converged;

    for (const QValueInfo& info : nconv_qs) {
        if (set_min(v, info.compute_q_value())) {
            best_action = info.action;
        }
    }

    if constexpr (UseInterval) {
        update(*value, v);
        return !value->bounds_approximately_equal();
    } else {
        return update(*value, v);
    }
}

template <typename State, typename Action, bool UseInterval>
TopologicalValueIteration<State, Action, UseInterval>::
    TopologicalValueIteration(bool expand_goals)
    : expand_goals_(expand_goals)
{
}

template <typename State, typename Action, bool UseInterval>
auto TopologicalValueIteration<State, Action, UseInterval>::compute_policy(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    ProgressReport,
    double max_time) -> std::unique_ptr<Policy>
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
    MDP& mdp,
    Evaluator& heuristic,
    StateID init_state_id,
    ValueStore& value_store,
    double max_time,
    MapPolicy* policy)
{
    utils::CountdownTimer timer(max_time);

    StateInfo& iinfo = state_information_[init_state_id];
    AlgorithmValueType& init_value = value_store[init_state_id];

    if (!push_state(mdp, heuristic, init_state_id, iinfo, init_value)) {
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
            mdp,
            heuristic,
            *explore,
            *stack_info,
            state_id,
            value_store,
            timer)) {
            explore = &exploration_stack_.back();
            state_id = explore->state_id;
            stack_info = &stack_[explore->stackidx];
        }

        do {
            // Check if an SCC was found.
            const unsigned stack_id = explore->stackidx;
            const unsigned lowlink = explore->lowlink;
            const bool onstack = stack_id != lowlink;

            if (!onstack) {
                const auto begin = stack_.begin() + stack_id;
                const auto end = stack_.end();
                scc_found(begin, end, policy, timer);
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
            state_id = explore->state_id;
            stack_info = &stack_[explore->stackidx];

            const auto [succ_id, prob] = explore->get_current_successor();
            AlgorithmValueType& s_value = value_store[succ_id];
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

            if (tinfo.finalize_transition()) {
                set_min(stack_info->conv_part, tinfo.conv_part);
                stack_info->nconv_qs.pop_back();
            }
        } while (!explore->next_transition(mdp, *stack_info, state_id));
    }
}

template <typename State, typename Action, bool UseInterval>
bool TopologicalValueIteration<State, Action, UseInterval>::push_state(
    MDP& mdp,
    Evaluator& heuristic,
    StateID state_id,
    StateInfo& state_info,
    AlgorithmValueType& state_value)
{
    assert(state_info.status == StateInfo::NEW);

    const State state = mdp.get_state(state_id);

    const TerminationInfo state_eval = mdp.get_termination_info(state);
    const value_t t_cost = state_eval.get_cost();
    const value_t estimate = heuristic.evaluate(state);

    if (state_eval.is_goal_state()) {
        ++statistics_.goal_states;

        if (!expand_goals_) {
            ++statistics_.terminal_states;

            state_value = AlgorithmValueType(0_vt);
            state_info.status = StateInfo::CLOSED;

            return false;
        }
    } else if (estimate == t_cost) {
        ++statistics_.pruned;

        state_value = AlgorithmValueType(estimate);
        state_info.status = StateInfo::CLOSED;

        return false;
    }

    state_info.status = StateInfo::ONSTACK;

    std::vector<Action> aops;
    mdp.generate_applicable_actions(state, aops);
    ++statistics_.expanded_states;

    if (aops.empty()) {
        ++statistics_.terminal_states;

        state_value = AlgorithmValueType(t_cost);
        state_info.status = StateInfo::CLOSED;

        return false;
    }

    /************** Forward to first non self loop ****************/
    Distribution<StateID> transition;

    do {
        Action& current_op = aops.back();

        mdp.generate_action_transitions(state, aops.back(), transition);

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
            auto& s_info =
                stack_.emplace_back(state_id, state_value, t_cost, aops.size());

            s_info.nconv_qs.emplace_back(
                current_op,
                mdp.get_action_cost(current_op));

            exploration_stack_.emplace_back(
                std::move(aops),
                std::move(transition),
                state_id,
                stack_size);

            return true;
        }

        aops.pop_back();
        transition.clear();
    } while (!aops.empty());
    /*****************************************************************/

    state_value = AlgorithmValueType(t_cost);
    state_info.status = StateInfo::CLOSED;

    return false;
}

template <typename State, typename Action, bool UseInterval>
template <typename ValueStore>
bool TopologicalValueIteration<State, Action, UseInterval>::successor_loop(
    MDP& mdp,
    Evaluator& heuristic,
    ExplorationInfo& explore,
    StackInfo& stack_info,
    StateID state_id,
    ValueStore& value_store,
    utils::CountdownTimer& timer)
{
    do {
        assert(!stack_info.nconv_qs.empty());

        QValueInfo& tinfo = stack_info.nconv_qs.back();

        do {
            timer.throw_if_expired();

            const auto [succ_id, prob] = explore.get_current_successor();

            if (succ_id == state_id) {
                tinfo.self_loop_prob += prob;
                continue;
            }

            StateInfo& succ_info = state_information_[succ_id];
            AlgorithmValueType& s_value = value_store[succ_id];

            switch (succ_info.status) {
            case StateInfo::NEW:
                if (push_state(mdp, heuristic, succ_id, succ_info, s_value)) {
                    return true; // recursion on new state
                }

                [[fallthrough]];
            case StateInfo::CLOSED: tinfo.conv_part += prob * s_value; break;

            case StateInfo::ONSTACK:
                explore.update_lowlink(succ_info.stack_id);
                tinfo.nconv_successors.emplace_back(&s_value, prob);
            }
        } while (explore.next_successor());

        if (tinfo.finalize_transition()) {
            if (set_min(stack_info.conv_part, tinfo.conv_part)) {
                stack_info.best_converged = tinfo.action;
            }
            stack_info.nconv_qs.pop_back();
        }
    } while (explore.next_transition(mdp, stack_info, state_id));

    return false;
}

template <typename State, typename Action, bool UseInterval>
void TopologicalValueIteration<State, Action, UseInterval>::scc_found(
    StackIterator begin,
    StackIterator end,
    MapPolicy* policy,
    utils::CountdownTimer& timer)
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
        bool converged;

        do {
            timer.throw_if_expired();

            converged = true;
            auto it = begin;

            do {
                if (it->update_value()) converged = false;
                ++statistics_.bellman_backups;
            } while (++it != end);
        } while (!converged);

        // Extract a policy from this SCC
        if (policy) {
            auto it = begin;

            do {
                if constexpr (UseInterval) {
                    policy->emplace_decision(
                        it->state_id,
                        *it->best_action,
                        *it->value);
                } else {
                    policy->emplace_decision(
                        it->state_id,
                        *it->best_action,
                        Interval(*it->value, INFINITE_VALUE));
                }
            } while (++it != end);
        }
    }

    stack_.erase(begin, end);
}

} // namespace topological_vi
} // namespace algorithms
} // namespace probfd