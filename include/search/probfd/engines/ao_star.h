#ifndef PROBFD_ENGINES_AO_STAR_H
#define PROBFD_ENGINES_AO_STAR_H

#include "probfd/engines/ao_search.h"

#include "probfd/engine_interfaces/successor_sampler.h"

#include "downward/utils/countdown_timer.h"

#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

/// Namespace dedicated to the AO* algorithm.
namespace ao_search {
namespace ao_star {

/**
 * @brief Implementation of the AO* algorithm.
 *
 * The AO* algorithm is an MDP heuristic search algorithm applicable only to
 * acyclic MDPs. The algorithm maintains a greedy policy and iteratively
 * constructs a subgraph of the MDP. In each iteration, a non-terminal leaf
 * stateid of the subgraph that is reachable by the greedy policy (a tip
 * stateid) is sampled and expanded. Afterwards, Bellman updates are performed
 * on the backward-reachable subgraph ending in the sampled stateid in reverse
 * topological order, while also updating the greedy policy. The algorithm
 * terminates once no tip states are left, i.e. the greedy policy is fully
 * explored.
 *
 * @tparam State - The stateid type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 *
 * @remark The search engine does not validate that the stateid space is
 * acyclic. It is an error to invoke this search engine on stateid spaces which
 * contain cycles.
 */
template <typename State, typename Action, bool UseInterval>
class AOStar
    : public AOBase<State, Action, UseInterval, true, PerStateInformation> {
    using Base = typename AOStar::AOBase;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;
    using PolicyPicker = typename Base::PolicyPicker;
    using NewStateObserver = typename Base::NewStateObserver;

    using SuccessorSampler = engine_interfaces::SuccessorSampler<Action>;

    SuccessorSampler* outcome_selection_;
    std::vector<Distribution<StateID>> transitions_;

public:
    AOStar(
        PolicyPicker* policy_chooser,
        NewStateObserver* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        SuccessorSampler* outcome_selection)
        : Base(policy_chooser, new_state_handler, report, interval_comparison)
        , outcome_selection_(outcome_selection)
    {
    }

protected:
    Interval do_solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time) override
    {
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
                        this->mark_solved_push_parents(stateid, info, dead);
                        this->backpropagate_tip_value(mdp, heuristic, timer);
                        break;
                    }

                    unsigned min_succ_order =
                        std::numeric_limits<unsigned>::max();

                    ClearGuard _guard(this->aops_, this->transitions_);

                    mdp.generate_all_transitions(
                        stateid,
                        this->aops_,
                        this->transitions_);

                    for (const auto& transition : this->transitions_) {
                        for (StateID succ_id : transition.support()) {
                            auto& succ_info = this->get_state_info(succ_id);

                            if (succ_info.is_unflagged()) {
                                assert(!succ_info.is_solved());
                                succ_info.mark();
                                succ_info.add_parent(stateid);
                                assert(
                                    succ_info.update_order <
                                    std::numeric_limits<unsigned>::max());
                                min_succ_order = std::min(
                                    min_succ_order,
                                    succ_info.update_order);
                            }
                        }
                    }

                    assert(
                        min_succ_order < std::numeric_limits<unsigned>::max());

                    info.update_order = min_succ_order + 1;

                    for (const auto& transition : this->transitions_) {
                        for (const StateID succ_id : transition.support()) {
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

                ClearGuard guard(this->selected_transition_);
                this->apply_policy(
                    mdp,
                    heuristic,
                    stateid,
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
            this->print_progress();
        }

        return iinfo.get_bounds();
    }
};

} // namespace ao_star
} // namespace ao_search
} // namespace engines
} // namespace probfd

#endif // __AO_STAR_H__