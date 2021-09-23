#ifndef MDPS_ENGINES_AO_STAR_H
#define MDPS_ENGINES_AO_STAR_H

#include "../engine_interfaces/transition_sampler.h"
#include "ao_search.h"

#include <iostream>
#include <queue>
#include <type_traits>
#include <vector>

namespace probabilistic {
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
 * @tparam StateT - The state type of the underlying MDP.
 * @tparam ActionT - The action type of the underlying MDP.
 * @tparam DualBoundsT - Whether bounded value iteration shall be used.
 *
 * @remark Does not validate that the input model is acyclic.
 */
template <typename StateT, typename ActionT, typename DualBoundsT>
class AOStar : public internal::AOBase<StateT, ActionT, DualBoundsT> {
public:
    /// Base class typedef.
    using AOBase = internal::AOBase<StateT, ActionT, DualBoundsT>;

    /// The underlying state type.
    using State = typename AOBase::State;

    /// The underlying action type.
    using Action = typename AOBase::Action;

    AOStar(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        DeadEndIdentificationLevel level,
        StateEvaluator<State>* dead_end_eval,
        DeadEndListener<State, Action>* dead_end_listener,
        PolicyPicker<Action>* policy_chooser,
        NewStateHandler<State>* new_state_handler,
        StateEvaluator<State>* value_init,
        HeuristicSearchConnector* connector,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy,
        TransitionSampler<Action>* outcome_selection)
        : AOBase(
              state_id_map,
              action_id_map,
              state_reward_function,
              action_reward_function,
              minimal_reward,
              maximal_reward,
              aops_generator,
              transition_generator,
              level,
              dead_end_eval,
              dead_end_listener,
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

    virtual void solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID stateid = this->get_state_id(state);
        auto& iinfo = this->state_infos_(stateid);
        iinfo.update_order = 0;
        while (!iinfo.is_solved()) {
            greedy_forward_exploration(stateid);
            ++this->statistics_.iterations;
            this->report(stateid);
        }
    }

private:
    using StateInfo = typename AOBase::StateInfo;

    void greedy_forward_exploration(StateID state)
    {
        do {
            auto& info = this->state_infos_(state);
            // std::cout << " --> " << stateid << "... ";
            assert(!info.is_solved());
            if (info.is_tip_state()) {
                // std::cout << " *tip*";
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
                } else if (solved) {
                    this->mark_solved_push_parents(state, info, dead);
                    this->backpropagate_tip_value();
                    break;
                }

                int min_succ_order = std::numeric_limits<int>::max();
                assert(this->aops_.empty() && this->transitions_.empty());
                this->generate_all_successors(
                    state,
                    this->aops_,
                    this->transitions_);
                for (int i = this->transitions_.size() - 1; i >= 0; --i) {
                    const auto& t = this->transitions_[i];
                    for (auto it = t.begin(); it != t.end(); ++it) {
                        const StateID succ_id = it->first;
                        auto& succ_info = this->state_infos_(succ_id);
                        if (succ_info.is_unflagged()) {
                            assert(!succ_info.is_solved());
                            succ_info.mark();
                            succ_info.add_parent(state);
                            assert(
                                succ_info.update_order >= 0 &&
                                succ_info.update_order <
                                    std::numeric_limits<int>::max());
                            min_succ_order = std::min(
                                min_succ_order,
                                succ_info.update_order);
                        }
                    }
                }
                assert(
                    min_succ_order >= 0 &&
                    min_succ_order < std::numeric_limits<int>::max());
                info.update_order = min_succ_order + 1;

                for (int i = this->transitions_.size() - 1; i >= 0; --i) {
                    const auto& t = this->transitions_[i];
                    for (auto it = t.begin(); it != t.end(); ++it) {
                        const StateID succ_id = it->first;
                        auto& succ_info = this->state_infos_(succ_id);
                        succ_info.unmark();
                    }
                }

                this->aops_.clear();
                this->transitions_.clear();

                this->backpropagate_update_order(state);

                if (value_changed) {
                    // std::cout << " *value changed*" << std::endl;
                    this->push_parents_to_queue(info);
                    this->backpropagate_tip_value();
                    break;
                }
            }
            // std::cout << " > [";

            assert(
                !info.is_tip_state() && !info.is_terminal() &&
                !info.is_solved());

            this->apply_policy(state, this->selected_transition_);

            value_type::value_t normalize_factor = 0;
            auto succ = this->selected_transition_.begin();
            while (succ != this->selected_transition_.end()) {
                if (this->state_infos_(succ->first).is_solved()) {
                    // std::cout << "-" <<
                    // this->state_id_map_->operator[](succ->first);
                    normalize_factor += succ->second;
                    succ = this->selected_transition_.erase(succ);
                } else {
                    // std::cout << "+" <<
                    // this->state_id_map_->operator[](succ->first);
                    ++succ;
                }
            }
            // std::cout << "]" << std::endl;
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

    TransitionSampler<Action>* outcome_selection_;
    std::vector<Distribution<StateID>> transitions_;
};

} // namespace ao_star
} // namespace engines
} // namespace probabilistic

#endif // __AO_STAR_H__