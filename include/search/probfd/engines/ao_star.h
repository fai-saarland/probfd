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
 * state of the subgraph that is reachable by the greedy policy (a tip state)
 * is sampled and expanded. Afterwards, Bellman updates are performed on the
 * backward-reachable subgraph ending in the sampled state in reverse
 * topological order, while also updating the greedy policy. The algorithm
 * terminates once no tip states are left, i.e. the greedy policy is fully
 * explored.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 *
 * @remark The search engine does not validate that the state space is acyclic.
 * It is an error to invoke this search engine on state spaces which contain
 * cycles.
 */
template <typename State, typename Action, bool UseInterval>
class AOStar
    : public AOBase<State, Action, UseInterval, true, PerStateInformation> {
    engine_interfaces::SuccessorSampler<Action>* outcome_selection_;
    std::vector<Distribution<StateID>> transitions_;

public:
    AOStar(
        MDP<State, Action>* mdp,
        Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<State, Action>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        engine_interfaces::SuccessorSampler<Action>* outcome_selection)
        : AOBase<State, Action, UseInterval, true, PerStateInformation>(
              mdp,
              value_init,
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison)
        , outcome_selection_(outcome_selection)
    {
    }

protected:
    Interval do_solve(param_type<State> state, double max_time) override
    {
        utils::CountdownTimer timer(max_time);

        const StateID stateid = this->get_state_id(state);
        auto& iinfo = this->get_state_info(stateid);
        iinfo.update_order = 0;

        while (!iinfo.is_solved()) {
            greedy_forward_exploration(stateid, timer);
            ++this->statistics_.iterations;
            this->print_progress();
        }

        return iinfo.get_bounds();
    }

private:
    void greedy_forward_exploration(StateID state, utils::CountdownTimer& timer)
    {
        for (;;) {
            timer.throw_if_expired();

            auto& info = this->get_state_info(state);
            assert(!info.is_solved());

            if (info.is_tip_state()) {
                bool solved = false;
                bool dead = false;
                bool terminal = false;
                bool value_changed = false;

                this->initialize_tip_state_value(
                    state,
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
                    this->mark_solved_push_parents(state, info, dead);
                    this->backpropagate_tip_value(timer);
                    break;
                }

                unsigned min_succ_order = std::numeric_limits<unsigned>::max();

                ClearGuard _guard(this->aops_, this->transitions_);

                this->generate_all_transitions(
                    state,
                    this->aops_,
                    this->transitions_);

                for (const auto& transition : this->transitions_) {
                    for (StateID succ_id : transition.support()) {
                        auto& succ_info = this->get_state_info(succ_id);

                        if (succ_info.is_unflagged()) {
                            assert(!succ_info.is_solved());
                            succ_info.mark();
                            succ_info.add_parent(state);
                            assert(
                                succ_info.update_order <
                                std::numeric_limits<unsigned>::max());
                            min_succ_order = std::min(
                                min_succ_order,
                                succ_info.update_order);
                        }
                    }
                }

                assert(min_succ_order < std::numeric_limits<unsigned>::max());

                info.update_order = min_succ_order + 1;

                for (const auto& transition : this->transitions_) {
                    for (const StateID succ_id : transition.support()) {
                        this->get_state_info(succ_id).unmark();
                    }
                }

                this->backpropagate_update_order(state, timer);

                if (value_changed) {
                    this->push_parents_to_queue(info);
                    this->backpropagate_tip_value(timer);
                    break;
                }
            }

            assert(
                !info.is_tip_state() && !info.is_terminal() &&
                !info.is_solved());

            ClearGuard guard(this->selected_transition_);
            this->apply_policy(state, this->selected_transition_);

            this->selected_transition_.remove_if_normalize(
                [this](const auto& target) {
                    return this->get_state_info(target.item).is_solved();
                });

            state = this->sample_state(
                *outcome_selection_,
                state,
                this->selected_transition_);
        }
    }
};

} // namespace ao_star
} // namespace ao_search
} // namespace engines
} // namespace probfd

#endif // __AO_STAR_H__