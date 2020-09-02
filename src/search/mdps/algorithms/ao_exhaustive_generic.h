#pragma once

#include "../interfaces/i_printable.h"
#include "ao_search.h"

#include <iostream>

namespace probabilistic {
namespace algorithms {
namespace exhaustive_ao {

namespace internal {

template<typename StateInfo>
struct PerStateInformation
    : public ao_search::internal::PerStateInformation<StateInfo> {
    PerStateInformation()
        : ao_search::internal::PerStateInformation<StateInfo>()
        , unsolved(0)
        , alive(0)
    {
    }
    unsigned unsolved : 31;
    unsigned alive : 1;
};

template<typename State, typename Action, typename DualBounds>
using AOBase = ao_search::AOBase<
    State,
    Action,
    DualBounds,
    std::false_type,
    internal::PerStateInformation,
    false>;

} // namespace internal

template<typename StateT, typename ActionT, typename DualBoundsT>
class GenericExhaustiveAOSearch
    : public internal::AOBase<StateT, ActionT, DualBoundsT> {
public:
    using AOBase = internal::AOBase<StateT, ActionT, DualBoundsT>;
    using State = typename AOBase::State;
    using Action = typename AOBase::Action;

    template<typename... Args>
    GenericExhaustiveAOSearch(OpenList<State, Action>* open_list, Args... args)
        : AOBase("Exhaustive AO", args...)
        , open_list_(open_list)
    {
    }

    virtual AnalysisResult solve(const State& state) override
    {
        this->initialize_report(state);
        StateID stateid = this->state_id_map_->operator[](state);
        const auto& state_info = this->state_infos_(stateid);
        open_list_->push(stateid, state);
        do {
            step();
            this->report(stateid);
        } while (!state_info.is_solved());
        return this->create_result(state);
    }

private:
    using StateInfo = typename AOBase::StateInfo;

    void step()
    {
        assert(!this->open_list_->empty());
        StateID stateid = this->open_list_->pop();
        auto& info = this->state_infos_(stateid);
        if (!info.is_tip_state() || info.is_solved()) {
            return;
        }
        ++this->stats_.iterations;
        State state = this->state_id_map_->operator[](stateid);
        bool solved = false;
        bool dead = false;
        bool terminal = false;
        bool value_changed = false;
        this->initialize_tip_state_value(
            state, info, terminal, solved, dead, value_changed);
        if (terminal) {
            assert(info.is_solved());
        } else {
            unsigned alive = 0;
            int min_succ_order = std::numeric_limits<int>::max();
            this->aops_gen_->operator()(state, this->aops_);
            assert(!this->aops_.empty());
            for (int i = this->aops_.size() - 1; i >= 0; i--) {
                this->selected_transition_ =
                    this->transition_gen_->operator()(state, this->aops_[i]);
                for (auto it = this->selected_transition_.begin();
                     it != this->selected_transition_.end();
                     it++) {
                    StateID succid = this->state_id_map_->operator[](it->first);
                    auto& succ_info = this->state_infos_(succid);
                    if (!succ_info.is_solved()) {
                        if (!succ_info.is_marked()) {
                            this->successors_.push_back(succid);
                            succ_info.mark();
                            succ_info.add_parent(stateid);
                            min_succ_order = std::min(
                                min_succ_order, succ_info.update_order);
                        }
                        this->open_list_->push(
                            succid,
                            state,
                            this->aops_[i],
                            it->second,
                            it->first);
                    } else if (!succ_info.is_dead_end()) {
                        ++alive;
                    }
                }
            }
            this->selected_transition_.clear();
            this->aops_.clear();
            info.alive = alive > 0;
            if (this->successors_.empty()) {
                this->mark_solved_push_parents(state, info, info.alive == 0);
                this->backpropagate_tip_value();
            } else {
                assert(
                    min_succ_order >= 0
                    && min_succ_order < std::numeric_limits<int>::max());
                info.update_order = min_succ_order + 1;
                info.unsolved = this->successors_.size();
                for (auto it = this->successors_.begin();
                     it != this->successors_.end();
                     ++it) {
                    this->state_infos_(*it).unmark();
                }
                this->successors_.clear();
                this->backpropagate_update_order(stateid);
                if (value_changed) {
                    this->push_parents_to_queue(info);
                    this->backpropagate_tip_value();
                }
            }
        }
    }

    OpenList<State, Action>* open_list_;
};

} // namespace exhaustive_ao
} // namespace algorithms
} // namespace probabilistic
