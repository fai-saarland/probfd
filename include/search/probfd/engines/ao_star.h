#ifndef MDPS_ENGINES_AO_STAR_H
#define MDPS_ENGINES_AO_STAR_H

#include "probfd/engine_interfaces/transition_sampler.h"
#include "probfd/engines/ao_search.h"

#include <iostream>
#include <queue>
#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

/// Namespace dedicated to the AO* algorithm.
namespace ao_star {

namespace internal {

template <typename State, typename Action, typename DualBounds>
using AOBase = ao_search::AOBase<
    State,
    Action,
    DualBounds,
    std::true_type,
    ao_search::PerStateInformation,
    true>;

}

/**
 * @brief Implementation of the AO* algorithm.
 *
 * The AO* algorithm is an MDP heuristic search algorithm applicable to acyclic
 * MDPs.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam DualBounds - Whether bounded value iteration shall be used.
 *
 * @remark Does not validate that the input model is acyclic.
 */
template <typename State, typename Action, typename DualBounds>
class AOStar : public internal::AOBase<State, Action, DualBounds> {
    using AOBase = internal::AOBase<State, Action, DualBounds>;
    using StateInfo = typename AOBase::StateInfo;

public:
    AOStar(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* reward_function,
        value_utils::IntervalValue reward_bound,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::StateEvaluator<State>* dead_end_eval,
        engine_interfaces::PolicyPicker<Action>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
        engine_interfaces::StateEvaluator<State>* value_init,
        engine_interfaces::HeuristicSearchConnector* connector,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy,
        engine_interfaces::TransitionSampler<Action>* outcome_selection)
        : AOBase(
              state_id_map,
              action_id_map,
              reward_function,
              reward_bound,
              transition_generator,
              dead_end_eval,
              policy_chooser,
              new_state_handler,
              value_init,
              connector,
              report,
              interval_comparison,
              stable_policy)
        , outcome_selection_(outcome_selection)
    {
    }

    virtual value_type::value_t solve(const State& state) override
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
                    for (const StateID& succ_id : transition.elements()) {
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

            value_type::value_t normalize_factor = 0;
            auto succ = this->selected_transition_.begin();
            while (succ != this->selected_transition_.end()) {
                if (this->get_state_info(succ->element).is_solved()) {
                    normalize_factor += succ->probability;
                    succ = this->selected_transition_.erase(succ);
                } else {
                    ++succ;
                }
            }

            assert(!this->selected_transition_.empty());

            if (normalize_factor > 0) {
                this->selected_transition_.normalize(
                    value_type::one / (value_type::one - normalize_factor));
            }

            state = this->outcome_selection_->operator()(
                state,
                this->get_policy(state),
                this->selected_transition_);

            this->selected_transition_.clear();
        } while (true);
    }

    engine_interfaces::TransitionSampler<Action>* outcome_selection_;
    std::vector<Distribution<StateID>> transitions_;
};

} // namespace ao_star
} // namespace engines
} // namespace probfd

#endif // __AO_STAR_H__