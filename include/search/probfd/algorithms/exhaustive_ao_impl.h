#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H
#error "This file should only be included from exhaustive_ao.h"
#endif

#include "probfd/algorithms/open_list.h"

#include "downward/utils/countdown_timer.h"

namespace probfd::algorithms::exhaustive_ao {

template <typename State, typename Action, bool UseInterval>
ExhaustiveAOSearch<State, Action, UseInterval>::ExhaustiveAOSearch(
    std::shared_ptr<PolicyPickerType> policy_chooser,
    std::shared_ptr<OpenListType> open_list)
    : Base(policy_chooser)
    , open_list_(open_list)
{
}

template <typename State, typename Action, bool UseInterval>
Interval ExhaustiveAOSearch<State, Action, UseInterval>::do_solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> initial_state,
    ProgressReport& progress,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    // Re-used buffer
    std::vector<Transition<Action>> transitions;

    StateID initstateid = mdp.get_state_id(initial_state);
    const auto& state_info = this->state_infos_[initstateid];
    open_list_->push(initstateid);

    do {
        timer.throw_if_expired();
        progress.print();

        assert(!this->open_list_->empty());
        StateID stateid = open_list_->pop();
        auto& info = this->state_infos_[stateid];

        if (!info.is_tip_state() || info.is_solved()) {
            continue;
        }

        ++this->statistics_.iterations;

        bool value_changed = this->bellman_update(mdp, heuristic, stateid);

        // Terminal non-goal state or pruned by heuristic
        if (info.is_solved()) {
            this->push_parents_to_queue(info);
            this->backpropagate_tip_value(mdp, heuristic, timer);
            continue;
        }

        ClearGuard _(transitions);
        const State state = mdp.get_state(stateid);
        mdp.generate_all_transitions(state, transitions);

        unsigned min_order = std::numeric_limits<unsigned>::max();

        for (const auto& [op, dist] : transitions) {
            for (auto& [succid, prob] : dist) {
                auto& succ_info = this->state_infos_[succid];
                if (succ_info.is_solved()) continue;

                open_list_->push(stateid, op, prob, succid);

                if (succ_info.is_marked()) continue;

                succ_info.mark();
                succ_info.add_parent(stateid);
                min_order = std::min(min_order, succ_info.update_order);
                ++info.unsolved;
            }
        }

        if (info.unsolved == 0) {
            assert(!info.is_terminal() && !info.is_solved());

            info.set_solved();
            this->push_parents_to_queue(info);
            this->backpropagate_tip_value(mdp, heuristic, timer);
            continue;
        }

        for (const auto& transition : transitions) {
            for (StateID succ_id : transition.successor_dist.support()) {
                this->state_infos_[succ_id].unmark();
            }
        }

        assert(min_order < std::numeric_limits<unsigned>::max());
        this->backpropagate_update_order(stateid, info, min_order + 1, timer);

        if (value_changed) {
            this->push_parents_to_queue(info);
            this->backpropagate_tip_value(mdp, heuristic, timer);
        }
    } while (!state_info.is_solved());

    return state_info.get_bounds();
}

template <typename State, typename Action, bool UseInterval>
bool ExhaustiveAOSearch<State, Action, UseInterval>::update_value_check_solved(
    MDPType& mdp,
    EvaluatorType& heuristic,
    StateID state,
    StateInfo& info)
{
    assert(!info.is_solved());

    const bool result = this->bellman_update(mdp, heuristic, state);

    if (info.unsolved == 0) {
        info.set_solved();
    }

    return result;
}

} // namespace probfd::algorithms::exhaustive_ao
