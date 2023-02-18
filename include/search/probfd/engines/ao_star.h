#ifndef PROBFD_ENGINES_AO_STAR_H
#define PROBFD_ENGINES_AO_STAR_H

#include "probfd/engine_interfaces/transition_sampler.h"
#include "probfd/engines/ao_search.h"

#include <iostream>
#include <queue>
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
 * The AO* algorithm is an MDP heuristic search algorithm applicable to acyclic
 * MDPs.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam Interval - Whether bounded value iteration shall be used.
 *
 * @remark Does not validate that the input model is acyclic.
 */
template <typename State, typename Action, bool Interval>
class AOStar
    : public AOBase<State, Action, Interval, true, PerStateInformation, true> {
    engine_interfaces::TransitionSampler<Action>* outcome_selection_;
    std::vector<Distribution<StateID>> transitions_;

public:
    AOStar(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<Action>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy,
        engine_interfaces::TransitionSampler<Action>* outcome_selection)
        : AOBase<State, Action, Interval, true, PerStateInformation, true>(
              state_id_map,
              action_id_map,
              transition_generator,
              cost_function,
              value_init,
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison,
              stable_policy)
        , outcome_selection_(outcome_selection)
    {
    }

    virtual value_t solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID stateid = this->get_state_id(state);
        auto& iinfo = this->get_state_info(stateid);
        iinfo.update_order = 0;

        while (!iinfo.is_solved()) {
            greedy_forward_exploration(stateid);
            ++this->statistics_.iterations;
            this->report(stateid);
        }

        return this->get_value(stateid);
    }

private:
    void greedy_forward_exploration(StateID state)
    {
        do {
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
                    value_changed);

                if (terminal) {
                    assert(info.is_solved());
                    break;
                }

                if (solved) {
                    this->mark_solved_push_parents(state, info, dead);
                    this->backpropagate_tip_value();
                    break;
                }

                unsigned min_succ_order = std::numeric_limits<unsigned>::max();

                assert(this->aops_.empty() && this->transitions_.empty());

                this->generate_all_successors(
                    state,
                    this->aops_,
                    this->transitions_);

                for (const auto& transition : this->transitions_) {
                    for (StateID succ_id : transition.elements()) {
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
                    for (const StateID succ_id : transition.elements()) {
                        this->get_state_info(succ_id).unmark();
                    }
                }

                this->aops_.clear();
                this->transitions_.clear();

                this->backpropagate_update_order(state);

                if (value_changed) {
                    this->push_parents_to_queue(info);
                    this->backpropagate_tip_value();
                    break;
                }
            }

            assert(
                !info.is_tip_state() && !info.is_terminal() &&
                !info.is_solved());

            this->apply_policy(state, this->selected_transition_);

            this->selected_transition_.remove_if_normalize(
                [this](const auto& target) {
                    return this->get_state_info(target.item).is_solved();
                });

            assert(!this->selected_transition_.empty());

            state = this->outcome_selection_->sample(
                state,
                this->get_policy(state),
                this->selected_transition_,
                *this);

            this->selected_transition_.clear();
        } while (true);
    }
};

} // namespace ao_star
} // namespace ao_search
} // namespace engines
} // namespace probfd

#endif // __AO_STAR_H__