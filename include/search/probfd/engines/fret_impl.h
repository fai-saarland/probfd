#ifndef GUARD_INCLUDE_PROBFD_ENGINES_FRET_H
#error "This file should only be included from fret.h"
#endif

#include "probfd/policies/map_policy.h"

#include "probfd/quotients/heuristic_search_interface.h"

#include "downward/utils/countdown_timer.h"

namespace probfd {
namespace engines {
namespace fret {

inline void Statistics::print(std::ostream& out) const
{
    out << "  FRET iterations: " << iterations << std::endl;
#if defined(EXPENSIVE_STATISTICS)
    out << "  Heuristic search: " << heuristic_search << std::endl;
    out << "  Trap identification: " << (trap_identification() - trap_removal())
        << std::endl;
    out << "  Trap removal: " << trap_removal << std::endl;
#endif
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
FRET<State, Action, UseInterval, GreedyGraphGenerator>::FRET(
    ProgressReport* report,
    std::shared_ptr<QHeuristicSearchEngine> engine)
    : base_engine_(std::move(engine))
{
    report->register_print([&](std::ostream& out) {
        out << "fret=" << statistics_.iterations
            << ", traps=" << statistics_.traps;
    });
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
auto FRET<State, Action, UseInterval, GreedyGraphGenerator>::compute_policy(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    double max_time) -> std::unique_ptr<Policy>
{
    QuotientSystem quotient(mdp);
    quotients::QuotientMaxHeuristic<State, Action> qheuristic(heuristic);
    this->solve(
        quotient,
        qheuristic,
        quotient.translate_state(state),
        max_time);

    /*
     * The quotient policy only specifies the optimal actions between traps.
     * We need to supplement the optimal actions within the traps, i.e.
     * the actions which point every other member state of the trap towards
     * that trap member state that owns the optimal quotient action.
     *
     * We fully explore the quotient policy starting from the initial state
     * and compute the optimal 'inner' actions for each trap. To this end,
     * we first generate the sub-MDP of the trap. Afterwards, we expand the
     * trap graph backwards from the state that has the optimal quotient
     * action. For each encountered state, we select the action with which
     * it is encountered first as the policy action.
     */

    std::unique_ptr<policies::MapPolicy<State, Action>> policy(
        new policies::MapPolicy<State, Action>(&mdp));

    const StateID initial_state_id = mdp.get_state_id(state);

    std::deque<StateID> queue;
    std::set<StateID> visited;
    queue.push_back(initial_state_id);
    visited.insert(initial_state_id);

    do {
        const StateID quotient_id = queue.front();
        const QState quotient_state = quotient.get_state(quotient_id);
        queue.pop_front();

        std::optional quotient_action =
            base_engine_->get_greedy_action(quotient_id);

        // Terminal states have no policy decision.
        if (!quotient_action) {
            continue;
        }

        const Interval quotient_bound =
            base_engine_->lookup_bounds(quotient_id);

        const StateID exiting_id = quotient_action->state_id;

        policy->emplace_decision(
            exiting_id,
            quotient_action->action,
            quotient_bound);

        // Nothing else needs to be done if the trap has only one state.
        if (quotient_state.num_members() != 1) {
            std::unordered_map<StateID, std::set<QAction>> parents;

            // Build the inverse graph
            std::vector<QAction> inner_actions;
            quotient_state.get_collapsed_actions(inner_actions);

            for (const QAction& qaction : inner_actions) {
                StateID source_id = qaction.state_id;
                Action action = qaction.action;

                const State source = mdp.get_state(source_id);

                Distribution<StateID> successors;
                mdp.generate_action_transitions(source, action, successors);

                for (const StateID succ_id : successors.support()) {
                    parents[succ_id].insert(qaction);
                }
            }

            // Now traverse the inverse graph starting from the exiting
            // state
            std::deque<StateID> inverse_queue;
            std::set<StateID> inverse_visited;
            inverse_queue.push_back(exiting_id);
            inverse_visited.insert(exiting_id);

            do {
                const StateID next_id = inverse_queue.front();
                inverse_queue.pop_front();

                for (const auto& [pred_id, act] : parents[next_id]) {
                    if (inverse_visited.insert(pred_id).second) {
                        policy->emplace_decision(pred_id, act, quotient_bound);
                        inverse_queue.push_back(pred_id);
                    }
                }
            } while (!inverse_queue.empty());
        }

        // Push the successor traps.
        Distribution<StateID> successors;
        quotient.generate_action_transitions(
            quotient_state,
            *quotient_action,
            successors);

        for (const StateID succ_id : successors.support()) {
            if (visited.insert(succ_id).second) {
                queue.push_back(succ_id);
            }
        }
    } while (!queue.empty());

    return policy;
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
Interval FRET<State, Action, UseInterval, GreedyGraphGenerator>::solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    double max_time)
{
    QuotientSystem quotient(mdp);
    quotients::QuotientMaxHeuristic<State, Action> qheuristic(heuristic);
    return solve(
        quotient,
        qheuristic,
        quotient.translate_state(state),
        max_time);
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
void FRET<State, Action, UseInterval, GreedyGraphGenerator>::print_statistics(
    std::ostream& out) const
{
    this->base_engine_->print_statistics(out);
    statistics_.print(out);
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
Interval FRET<State, Action, UseInterval, GreedyGraphGenerator>::solve(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    param_type<QState> state,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    for (;;) {
        const Interval value =
            heuristic_search(quotient, heuristic, state, timer);

        if (find_and_remove_traps(quotient, heuristic, state, timer)) {
            return value;
        }

        base_engine_->reset_search_state();
    }
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
Interval
FRET<State, Action, UseInterval, GreedyGraphGenerator>::heuristic_search(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    param_type<QState> state,
    utils::CountdownTimer& timer)
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped(statistics_.heuristic_search);
#endif
    return base_engine_
        ->solve(quotient, heuristic, state, timer.get_remaining_time());
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
bool FRET<State, Action, UseInterval, GreedyGraphGenerator>::
    find_and_remove_traps(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        param_type<QState> state,
        utils::CountdownTimer& timer)
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope scoped(statistics_.trap_identification);
#endif
    unsigned int trap_counter = 0;
    unsigned int unexpanded = 0;

    storage::StateHashMap<TarjanStateInformation> state_infos;
    std::deque<ExplorationInfo> exploration_queue;
    std::deque<StackInfo> stack;

    StateID state_id = quotient.get_state_id(state);
    TarjanStateInformation* sinfo = &state_infos[state_id];

    if (!push(
            quotient,
            heuristic,
            exploration_queue,
            stack,
            *sinfo,
            state_id,
            unexpanded)) {
        return unexpanded == 0;
    }

    ExplorationInfo* einfo = &exploration_queue.back();

    for (;;) {
        do {
            timer.throw_if_expired();

            const StateID succid = einfo->successors.back();
            TarjanStateInformation& succ_info = state_infos[succid];

            if (succ_info.is_on_stack()) {
                sinfo->lowlink =
                    std::min(sinfo->lowlink, succ_info.stack_index);
            } else if (
                !succ_info.is_explored() && push(
                                                quotient,
                                                heuristic,
                                                exploration_queue,
                                                stack,
                                                succ_info,
                                                succid,
                                                unexpanded)) {
                einfo = &exploration_queue.back();
                state_id = einfo->state_id;
                sinfo = &state_infos[state_id];
                continue;
            } else {
                einfo->is_leaf = false;
            }

            einfo->successors.pop_back();
        } while (!einfo->successors.empty());

        do {
            const unsigned last_lowlink = sinfo->lowlink;
            const bool scc_found = last_lowlink == sinfo->stack_index;
            const bool can_reach_child_scc = scc_found || !einfo->is_leaf;

            if (scc_found) {
                auto scc = stack | std::views::drop(sinfo->stack_index);

                for (const auto& info : scc) {
                    state_infos[info.state_id].close();
                }

                if (einfo->is_leaf) {
                    // Terminal and self-loop leaf SCCs are always pruned
                    assert(scc.size() > 1);
                    collapse_trap(quotient, scc);
                    base_engine_->bellman_policy_update(
                        quotient,
                        heuristic,
                        state_id);
                    ++trap_counter;
                }

                stack.erase(scc.begin(), scc.end());
            }

            exploration_queue.pop_back();

            if (exploration_queue.empty()) {
                ++statistics_.iterations;
                return trap_counter == 0 && unexpanded == 0;
            }

            timer.throw_if_expired();

            einfo = &exploration_queue.back();
            state_id = einfo->state_id;
            sinfo = &state_infos[state_id];

            sinfo->lowlink = std::min(sinfo->lowlink, last_lowlink);
            if (can_reach_child_scc) {
                einfo->is_leaf = false;
            }

            einfo->successors.pop_back();
        } while (einfo->successors.empty());
    }
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
void FRET<State, Action, UseInterval, GreedyGraphGenerator>::collapse_trap(
    QuotientSystem& quotient,
    auto scc)
{
#if defined(EXPENSIVE_STATISTICS)
    TimerScope t(statistics_.trap_removal);
#endif

    // Now collapse the quotient
    quotient.build_quotient(scc, *scc.begin());
    base_engine_->clear_policy(scc.begin()->state_id);

    ++statistics_.traps;
}

template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
bool FRET<State, Action, UseInterval, GreedyGraphGenerator>::push(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    std::deque<ExplorationInfo>& queue,
    std::deque<StackInfo>& stack,
    TarjanStateInformation& info,
    StateID state_id,
    unsigned int& unexpanded)
{
    if (base_engine_->is_terminal(state_id)) {
        return false;
    }

    GreedyGraphGenerator greedy_graph;
    std::vector<QAction> aops;
    std::vector<StateID> succs;
    if (greedy_graph.get_successors(
            quotient,
            heuristic,
            *base_engine_,
            state_id,
            aops,
            succs)) {
        ++unexpanded;
    }

    if (succs.empty()) {
        return false;
    }

    info.open(stack.size());
    stack.emplace_back(state_id, std::move(aops));
    queue.emplace_back(state_id, std::move(succs));
    return true;
}

template <typename State, typename Action, bool UseInterval>
bool ValueGraph<State, Action, UseInterval>::get_successors(
    QuotientSystem& quotient,
    QEvaluator& heuristic,
    QHeuristicSearchEngine& base_engine,
    StateID qstate,
    std::vector<QAction>& aops,
    std::vector<StateID>& successors)
{
    assert(successors.empty());

    ClearGuard _guard(ids, opt_transitions);

    bool value_changed = base_engine.bellman_update(
        quotient,
        heuristic,
        qstate,
        opt_transitions);

    for (const auto& transition : opt_transitions) {
        aops.push_back(transition.action);

        for (const StateID sid : transition.successor_dist.support()) {
            if (ids.insert(sid).second) {
                successors.push_back(sid);
            }
        }
    }

    return value_changed;
}

template <typename State, typename Action, bool UseInterval>
bool PolicyGraph<State, Action, UseInterval>::get_successors(
    QuotientSystem& quotient,
    QEvaluator&,
    QHeuristicSearchEngine& base_engine,
    StateID quotient_state_id,
    std::vector<QAction>& aops,
    std::vector<StateID>& successors)
{
    std::optional a = base_engine.get_greedy_action(quotient_state_id);
    assert(a.has_value());

    ClearGuard _guard(t_);

    const QState quotient_state = quotient.get_state(quotient_state_id);
    quotient.generate_action_transitions(quotient_state, *a, t_);

    for (StateID sid : t_.support()) {
        successors.push_back(sid);
    }

    aops.push_back(*a);

    return false;
}

} // namespace fret
} // namespace engines
} // namespace probfd