#pragma once

#include "ao_search.h"

#include <iostream>
#include <queue>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace algorithms {
namespace ao_star {

namespace internal {
template<typename State, typename Action, typename DualBounds>
using AOBase = ao_search::AOBase<
    State,
    Action,
    DualBounds,
    std::true_type,
    ao_search::internal::PerStateInformation,
    true>;
}

template<typename StateT, typename ActionT, typename DualBoundsT>
class AOStar : public internal::AOBase<StateT, ActionT, DualBoundsT> {
public:
    using AOBase = internal::AOBase<StateT, ActionT, DualBoundsT>;
    using State = typename AOBase::State;
    using Action = typename AOBase::Action;

    template<typename... Args>
    AOStar(TransitionSampler<State, Action>* outcome_selection, Args... args)
        : AOBase("AOStar", args...)
        , outcome_selection_(outcome_selection)
    {
        outcome_selection_->connect(this->interface());
    }

    virtual AnalysisResult solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID stateid = this->state_id_map_->operator[](state);
        auto& iinfo = this->state_infos_(stateid);
        iinfo.update_order = 0;
        while (!iinfo.is_solved()) {
            greedy_forward_exploration(state);
            ++this->stats_.iterations;
            this->report(stateid);
        }
        return this->create_result(state);
    }

private:
    using StateInfo = typename AOBase::StateInfo;

    void greedy_forward_exploration(State state)
    {
        do {
            StateID stateid = this->state_id_map_->operator[](state);
            auto& info = this->state_infos_(stateid);
            // std::cout << " --> " << stateid << "... ";
            assert(!info.is_solved());
            if (info.is_tip_state()) {
                // std::cout << " *tip*";
                bool solved = false;
                bool dead = false;
                bool terminal = false;
                bool value_changed = false;
                this->initialize_tip_state_value(
                    state, info, terminal, solved, dead, value_changed);
                if (terminal) {
                    assert(info.is_solved());
                    break;
                } else if (solved) {
                    this->mark_solved_push_parents(state, info, dead);
                    this->backpropagate_tip_value();
                    break;
                }

                int min_succ_order = std::numeric_limits<int>::max();
                this->aops_gen_->operator()(state, this->aops_);
                assert(!this->aops_.empty());
                for (int i = this->aops_.size() - 1; i >= 0; i--) {
                    this->selected_transition_ =
                        this->transition_gen_->operator()(
                            state, this->aops_[i]);
                    for (auto it = this->selected_transition_.begin();
                         it != this->selected_transition_.end();
                         ++it) {
                        StateID succ_id =
                            this->state_id_map_->operator[](it->first);
                        auto& succ_info = this->state_infos_(succ_id);
                        if (succ_info.is_unflagged()) {
                            assert(!succ_info.is_solved());
                            succ_info.mark();
                            succ_info.add_parent(stateid);
                            assert(
                                succ_info.update_order >= 0
                                && succ_info.update_order
                                    < std::numeric_limits<int>::max());
                            min_succ_order = std::min(
                                min_succ_order, succ_info.update_order);
                            this->successors_.push_back(succ_id);
                        }
                    }
                }
                this->selected_transition_.clear();
                this->aops_.clear();

                assert(
                    min_succ_order >= 0
                    && min_succ_order < std::numeric_limits<int>::max());
                info.update_order = min_succ_order + 1;

                for (int i = this->successors_.size() - 1; i >= 0; --i) {
                    auto& succ_info = this->state_infos_(this->successors_[i]);
                    succ_info.unmark();
                }
                this->successors_.clear();

                this->backpropagate_update_order(stateid);

                if (value_changed) {
                    // std::cout << " *value changed*" << std::endl;
                    this->push_parents_to_queue(info);
                    this->backpropagate_tip_value();
                    break;
                }
            }
            // std::cout << " > [";

            assert(
                !info.is_tip_state() && !info.is_terminal()
                && !info.is_solved());

            const Action& a = info.policy;
            this->selected_transition_ =
                this->transition_gen_->operator()(state, a);
            value_type::value_t normalize_factor = value_type::zero;
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
            if (normalize_factor > value_type::zero) {
                this->selected_transition_.normalize(
                    value_type::one / (value_type::one - normalize_factor));
            }
            state = this->outcome_selection_->operator()(
                state, a, this->selected_transition_);
            this->selected_transition_.clear();
        } while (true);
    }

    TransitionSampler<State, Action>* outcome_selection_;
};

} // namespace ao_star
} // namespace algorithms
} // namespace probabilistic
