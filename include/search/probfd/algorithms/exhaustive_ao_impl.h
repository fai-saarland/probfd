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

    StateID initstateid = mdp.get_state_id(initial_state);
    const auto& state_info = this->get_state_info(initstateid);
    open_list_->push(initstateid);

    do {
        timer.throw_if_expired();
        progress.print();

        assert(!this->open_list_->empty());
        StateID stateid = open_list_->pop();
        auto& info = this->get_state_info(stateid);

        if (!info.is_tip_state() || info.is_solved()) {
            continue;
        }

        ++this->statistics_.iterations;

        bool solved = false;
        bool dead = false;
        bool terminal = false;
        bool value_changed = false;

        this->initialize_tip_state_value(
            mdp,
            heuristic,
            stateid,
            info,
            terminal,
            solved,
            dead,
            value_changed,
            timer);

        if (terminal) {
            assert(info.is_solved());
            continue;
        }

        unsigned alive = 0;
        unsigned unsolved = 0;
        unsigned min_succ_order = std::numeric_limits<unsigned>::max();

        ClearGuard _(this->transitions_);
        const State state = mdp.get_state(stateid);
        mdp.generate_all_transitions(state, this->transitions_);

        for (const auto& [op, dist] : this->transitions_) {
            for (auto& [succid, prob] : dist) {
                auto& succ_info = this->get_state_info(succid);
                if (!succ_info.is_solved()) {
                    if (!succ_info.is_marked()) {
                        succ_info.mark();
                        succ_info.add_parent(stateid);
                        min_succ_order =
                            std::min(min_succ_order, succ_info.update_order);
                        ++unsolved;
                    }

                    open_list_->push(stateid, op, prob, succid);
                } else if (!succ_info.is_dead_end()) {
                    ++alive;
                }
            }
        }

        info.alive = alive > 0;

        if (unsolved == 0) {
            this->mark_solved_push_parents(info, info.alive == 0);
            this->backpropagate_tip_value(mdp, heuristic, timer);
            continue;
        }

        assert(min_succ_order < std::numeric_limits<unsigned>::max());
        info.update_order = min_succ_order + 1;
        info.unsolved = unsolved;

        for (const auto& transition : this->transitions_) {
            for (StateID succ_id : transition.successor_dist.support()) {
                this->get_state_info(succ_id).unmark();
            }
        }

        this->backpropagate_update_order(stateid, timer);

        if (value_changed) {
            this->push_parents_to_queue(info);
            this->backpropagate_tip_value(mdp, heuristic, timer);
        }
    } while (!state_info.is_solved());

    return state_info.get_bounds();
}

} // namespace probfd::algorithms::exhaustive_ao
