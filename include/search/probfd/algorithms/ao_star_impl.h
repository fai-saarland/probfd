#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_STAR_H
#error "This file should only be included from ao_star.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/progress_report.h"

#include "downward/utils/countdown_timer.h"

#include <ranges>
#include <type_traits>

namespace probfd {
namespace algorithms {
namespace ao_search {
namespace ao_star {

template <typename State, typename Action, bool UseInterval>
AOStar<State, Action, UseInterval>::AOStar(
    std::shared_ptr<PolicyPicker> policy_chooser,
    std::shared_ptr<SuccessorSampler> outcome_selection)
    : Base(policy_chooser)
    , outcome_selection_(outcome_selection)
{
}

template <typename State, typename Action, bool UseInterval>
Interval AOStar<State, Action, UseInterval>::do_solve(
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> state,
    ProgressReport& progress,
    double max_time)
{
    using namespace std::views;

    utils::CountdownTimer timer(max_time);

    const StateID initstateid = mdp.get_state_id(state);
    auto& iinfo = this->get_state_info(initstateid);
    iinfo.update_order = 0;

    while (!iinfo.is_solved()) {
        StateID stateid = initstateid;

        for (;;) {
            timer.throw_if_expired();

            auto& info = this->get_state_info(stateid);
            assert(!info.is_solved());

            if (info.is_tip_state()) {
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
                    break;
                }

                if (solved) {
                    this->mark_solved_push_parents(info, dead);
                    this->backpropagate_tip_value(mdp, heuristic, timer);
                    break;
                }

                constexpr auto UMAX = std::numeric_limits<unsigned>::max();

                unsigned min_succ_order = UMAX;

                ClearGuard _guard(this->transitions_);
                const State state = mdp.get_state(stateid);
                mdp.generate_all_transitions(state, this->transitions_);

                auto all_successors =
                    this->transitions_ | transform([](auto& t) {
                        return t.successor_dist.support();
                    });

                for (auto successors : all_successors) {
                    for (const StateID succ_id : successors) {
                        auto& succ_info = this->get_state_info(succ_id);

                        if (!succ_info.is_unflagged()) continue;

                        assert(!succ_info.is_solved());
                        succ_info.mark();
                        succ_info.add_parent(stateid);
                        assert(succ_info.update_order < UMAX);
                        min_succ_order =
                            std::min(min_succ_order, succ_info.update_order);
                    }
                }

                assert(min_succ_order < UMAX);

                info.update_order = min_succ_order + 1;

                for (auto successors : all_successors) {
                    for (const StateID succ_id : successors) {
                        this->get_state_info(succ_id).unmark();
                    }
                }

                this->backpropagate_update_order(stateid, timer);

                if (value_changed) {
                    this->push_parents_to_queue(info);
                    this->backpropagate_tip_value(mdp, heuristic, timer);
                    break;
                }
            }

            assert(
                !info.is_tip_state() && !info.is_terminal() &&
                !info.is_solved());

            const State state = mdp.get_state(stateid);

            ClearGuard guard(this->selected_transition_);
            mdp.generate_action_transitions(
                state,
                *this->get_greedy_action(stateid),
                this->selected_transition_);

            this->selected_transition_.remove_if_normalize(
                [this](const auto& target) {
                    return this->get_state_info(target.item).is_solved();
                });

            stateid = this->sample_state(
                *outcome_selection_,
                stateid,
                this->selected_transition_);
        }

        ++this->statistics_.iterations;
        progress.print();
    }

    return iinfo.get_bounds();
}

} // namespace ao_star
} // namespace ao_search
} // namespace algorithms
} // namespace probfd