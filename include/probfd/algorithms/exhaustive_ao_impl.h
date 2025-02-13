#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H
#error "This file should only be included from exhaustive_ao.h"
#endif

#include "probfd/algorithms/open_list.h"

#include "downward/utils/countdown_timer.h"

namespace probfd::algorithms::exhaustive_ao {

template <typename State, typename Action, bool UseInterval>
ExhaustiveAOSearch<State, Action, UseInterval>::ExhaustiveAOSearch(
    value_t epsilon,
    std::shared_ptr<PolicyPickerType> policy_chooser,
    std::shared_ptr<OpenListType> open_list)
    : Base(epsilon, policy_chooser)
    , open_list_(open_list)
{
}

template <typename State, typename Action, bool UseInterval>
Interval ExhaustiveAOSearch<State, Action, UseInterval>::do_solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> initial_state,
    ProgressReport& progress,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    StateID initstateid = mdp.get_state_id(initial_state);
    const auto& state_info = this->state_infos_[initstateid];

    open_list_->push(initstateid);

    progress.register_bound("v", [&state_info]() {
        return as_interval(state_info.value);
    });

    progress.register_print([&](std::ostream& out) {
        out << "i=" << this->statistics_.iterations;
    });

    do {
        timer.throw_if_expired();
        progress.print();

        assert(!this->open_list_->empty());
        StateID stateid = open_list_->pop();
        auto& info = this->state_infos_[stateid];

        if (!info.is_on_fringe() || info.is_solved()) {
            continue;
        }

        ++this->statistics_.iterations;

        const State state = mdp.get_state(stateid);

        ClearGuard _(transitions_);
        this->expand_and_initialize(mdp, heuristic, state, info, transitions_);

        const auto value = this->compute_bellman(state, transitions_, mdp);
        bool value_changed = this->update_value(info, value, this->epsilon);

        // Terminal state
        if (info.is_solved()) {
            assert(transitions_.empty());
            this->backpropagate_tip_value(mdp, transitions_, info, timer);
            continue;
        }

        unsigned min_order = std::numeric_limits<unsigned>::max();

        for (const auto& [op, dist] : transitions_) {
            for (auto& [succid, prob] : dist.non_source_successor_dist) {
                auto& succ_info = this->state_infos_[succid];
                if (succ_info.is_solved()) continue;

                open_list_->push(
                    stateid,
                    op,
                    prob / dist.non_source_probability,
                    succid);

                if (succ_info.is_marked()) continue;

                succ_info.mark();
                succ_info.add_parent(stateid);
                min_order = std::min(min_order, succ_info.update_order);
                ++info.unsolved;
            }
        }

        if (info.unsolved == 0) {
            transitions_.clear();
            info.set_solved();
            this->backpropagate_tip_value(mdp, transitions_, info, timer);
            continue;
        }

        for (const auto& transition : transitions_) {
            for (StateID succ_id : transition.successor_dist
                                       .non_source_successor_dist.support()) {
                this->state_infos_[succ_id].unmark();
            }
        }

        assert(min_order < std::numeric_limits<unsigned>::max());
        this->backpropagate_update_order(stateid, info, min_order + 1, timer);

        if (value_changed) {
            transitions_.clear();
            this->backpropagate_tip_value(mdp, transitions_, info, timer);
        }
    } while (!state_info.is_solved());

    return state_info.get_bounds();
}

template <typename State, typename Action, bool UseInterval>
bool ExhaustiveAOSearch<State, Action, UseInterval>::update_value_check_solved(
    MDPType& mdp,
    ParamType<State> state,
    std::vector<TransitionTail<Action>> transitions,
    StateInfo& info)
{
    assert(!info.is_solved());

    const auto value = this->compute_bellman(state, transitions, mdp);
    bool value_changed = this->update_value(info, value, this->epsilon);

    if (info.unsolved == 0) {
        info.set_solved();
    }

    return value_changed;
}

} // namespace probfd::algorithms::exhaustive_ao
