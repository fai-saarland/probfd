#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_STAR_H
#error "This file should only be included from ao_star.h"
#endif

#include "probfd/algorithms/utils.h"

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/progress_report.h"

#include "downward/utils/countdown_timer.h"

#include <ranges>
#include <type_traits>

namespace probfd::algorithms::ao_search::ao_star {

template <typename State, typename Action, bool UseInterval>
AOStar<State, Action, UseInterval>::AOStar(
    value_t epsilon,
    std::shared_ptr<PolicyPickerType> policy_chooser,
    std::shared_ptr<SuccessorSamplerType> outcome_selection)
    : Base(epsilon, std::move(policy_chooser))
    , outcome_selection_(std::move(outcome_selection))
{
}

template <typename State, typename Action, bool UseInterval>
Interval AOStar<State, Action, UseInterval>::do_solve(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> initial_state,
    ProgressReport& progress,
    downward::utils::Duration max_time)
{
    using namespace std::views;

    downward::utils::CountdownTimer timer(max_time);

    const StateID initstateid = mdp.get_state_id(initial_state);
    auto& iinfo = this->state_infos_[initstateid];

    progress.register_bound("v", [&iinfo]() {
        return as_interval(iinfo.value);
    });

    progress.register_print([&](std::ostream& out) {
        out << "i=" << this->statistics_.iterations;
    });

    iinfo.update_order = 0;

    for (; !iinfo.is_solved(); progress.print()) {
        StateID stateid = initstateid;

        for (;;) {
            timer.throw_if_expired();

            auto& info = this->state_infos_[stateid];
            assert(!info.is_solved());

            const State state = mdp.get_state(stateid);

            if (info.is_on_fringe()) {
                ClearGuard _(transitions_);
                this->expand_and_initialize(
                    mdp,
                    heuristic,
                    state,
                    info,
                    transitions_);

                bool value_changed = this->update_value_check_solved(
                    mdp,
                    state,
                    transitions_,
                    info);

                if (info.is_solved()) {
                    transitions_.clear();
                    this->backpropagate_tip_value(
                        mdp,
                        transitions_,
                        info,
                        timer);
                    break;
                }

                auto all_successors =
                    transitions_ | transform([](auto& t) {
                        return t.successor_dist.non_source_successor_dist
                            .support();
                    }) |
                    std::views::join;

                unsigned min_succ_order = std::numeric_limits<unsigned>::max();

                for (const StateID succ_id : all_successors) {
                    auto& succ_info = this->state_infos_[succ_id];

                    if (succ_info.is_marked() || succ_info.is_solved())
                        continue;

                    succ_info.mark();
                    succ_info.add_parent(stateid);
                    assert(
                        succ_info.update_order <
                        std::numeric_limits<unsigned>::max());
                    min_succ_order =
                        std::min(min_succ_order, succ_info.update_order);
                }

                assert(min_succ_order < std::numeric_limits<unsigned>::max());

                for (const StateID succ_id : all_successors) {
                    this->state_infos_[succ_id].unmark();
                }

                this->backpropagate_update_order(
                    stateid,
                    info,
                    min_succ_order + 1,
                    timer);

                if (value_changed) {
                    transitions_.clear();
                    this->backpropagate_tip_value(
                        mdp,
                        transitions_,
                        info,
                        timer);
                    break;
                }
            }

            assert(
                !info.is_on_fringe() && !info.is_goal_or_terminal() &&
                !info.is_solved());

            const auto action = info.get_policy();

            assert(action.has_value());

            ClearGuard guard(successor_dist_.non_source_successor_dist);

            mdp.generate_action_transitions(state, *action, successor_dist_);

            successor_dist_.non_source_successor_dist.remove_if_normalize(
                [this](const auto& target) {
                    return this->state_infos_[target.item].is_solved();
                });

            stateid = outcome_selection_->sample(
                stateid,
                *action,
                successor_dist_,
                this->state_infos_);
        }

        ++this->statistics_.iterations;
    }

    return iinfo.get_bounds();
}

template <typename State, typename Action, bool UseInterval>
void AOStar<State, Action, UseInterval>::print_additional_statistics(
    std::ostream& out) const
{
    Base::print_additional_statistics(out);
    this->outcome_selection_->print_statistics(out);
}

template <typename State, typename Action, bool UseInterval>
bool AOStar<State, Action, UseInterval>::update_value_check_solved(
    MDPType& mdp,
    ParamType<State> state,
    std::vector<TransitionTail<Action>> transitions,
    StateInfo& info)
{
    const auto value = this->compute_bellman_and_greedy(
        state,
        transitions,
        mdp,
        qvalues_);

    auto greedy_transition =
        this->select_greedy_transition(mdp, info.get_policy(), transitions_);

    bool value_changed = this->update_value(info, value, this->epsilon).changed;
    this->update_policy(info, greedy_transition);

    bool all_succs_solved = true;

    if (greedy_transition) {
        for (const auto succ_id : greedy_transition->successor_dist
                                      .non_source_successor_dist.support()) {
            const auto& succ_info = this->state_infos_[succ_id];
            all_succs_solved = all_succs_solved && succ_info.is_solved();
        }
    }

    if (all_succs_solved) {
        info.set_solved();
    }

    return value_changed;
}

} // namespace probfd::algorithms::ao_search::ao_star
