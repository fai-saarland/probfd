#ifndef MDPS_ENGINES_EXHAUSTIVE_AO_H
#define MDPS_ENGINES_EXHAUSTIVE_AO_H

#include "probfd/engines/ao_search.h"

#include "probfd/engine_interfaces/open_list.h"

#include <iostream>
#include <sstream>

namespace probfd {
namespace engines {

/// I do not know this algorithm.
namespace exhaustive_ao {

namespace internal {

template <typename StateInfo>
struct PerStateInformation : public ao_search::PerStateInformation<StateInfo> {
    PerStateInformation()
        : unsolved(0)
        , alive(0)
    {
    }
    unsigned unsolved : 31;
    unsigned alive : 1;
};

template <typename State, typename Action, typename DualBounds>
using AOBase = ao_search::AOBase<
    State,
    Action,
    DualBounds,
    std::false_type,
    PerStateInformation,
    false>;

} // namespace internal

/**
 * @brief Exhaustive AO* search algorithm.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam DualBounds - Whether bounded value iteration is used.
 */
template <typename State, typename Action, typename DualBounds>
class ExhaustiveAOSearch : public internal::AOBase<State, Action, DualBounds> {
    using AOBase = internal::AOBase<State, Action, DualBounds>;

public:
    ExhaustiveAOSearch(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* reward_function,
        value_utils::IntervalValue reward_bound,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::PolicyPicker<Action>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
        engine_interfaces::StateEvaluator<State>* value_init,
        engine_interfaces::HeuristicSearchConnector* connector,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy,
        engine_interfaces::OpenList<Action>* open_list)
        : AOBase(
              state_id_map,
              action_id_map,
              reward_function,
              reward_bound,
              transition_generator,
              policy_chooser,
              new_state_handler,
              value_init,
              connector,
              report,
              interval_comparison,
              stable_policy)
        , open_list_(open_list)
    {
    }

    virtual value_type::value_t solve(const State& state) override
    {
        this->initialize_report(state);
        StateID stateid = this->get_state_id(state);
        const auto& state_info = this->get_state_info(stateid);
        open_list_->push(stateid);

        do {
            step();
            this->report(stateid);
        } while (!state_info.is_solved());

        return this->get_value(stateid);
    }

private:
    void step()
    {
        assert(!this->open_list_->empty());
        StateID stateid = open_list_->pop();
        auto& info = this->get_state_info(stateid);
        if (!info.is_tip_state() || info.is_solved()) {
            return;
        }

        ++this->statistics_.iterations;

        bool solved = false;
        bool dead = false;
        bool terminal = false;
        bool value_changed = false;

        this->initialize_tip_state_value(
            stateid,
            info,
            terminal,
            solved,
            dead,
            value_changed);

        if (terminal) {
            assert(info.is_solved());
            return;
        }

        unsigned alive = 0;
        unsigned unsolved = 0;
        unsigned min_succ_order = std::numeric_limits<unsigned>::max();

        assert(this->aops_.empty() && this->transitions_.empty());

        this->generate_all_successors(stateid, this->aops_, this->transitions_);

        assert(this->aops_.size() == this->transitions_.size());

        for (std::size_t i = 0; i != this->aops_.size(); ++i) {
            auto& op = this->aops_[i];
            for (auto& [succid, prob] : this->transitions_[i]) {
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
            this->aops_.clear();
            this->transitions_.clear();

            this->mark_solved_push_parents(stateid, info, info.alive == 0);
            this->backpropagate_tip_value();
        } else {
            assert(min_succ_order < std::numeric_limits<unsigned>::max());
            info.update_order = min_succ_order + 1;
            info.unsolved = unsolved;

            for (const auto& transition : transitions_) {
                for (const StateID& succ_id : transition.elements()) {
                    this->get_state_info(succ_id).unmark();
                }
            }

            this->aops_.clear();
            this->transitions_.clear();

            this->backpropagate_update_order(stateid);

            if (value_changed) {
                this->push_parents_to_queue(info);
                this->backpropagate_tip_value();
            }
        }
    }

    engine_interfaces::OpenList<Action>* open_list_;
    std::vector<Distribution<StateID>> transitions_;
};

} // namespace exhaustive_ao
} // namespace engines
} // namespace probfd

#endif // __EXHAUSTIVE_AO_H__