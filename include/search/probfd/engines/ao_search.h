#ifndef PROBFD_ENGINES_AO_SEARCH_H
#define PROBFD_ENGINES_AO_SEARCH_H

#include "probfd/engines/heuristic_search_base.h"

#include "probfd/storage/per_state_storage.h"

#include "downward/utils/countdown_timer.h"

#include <limits>
#include <queue>
#include <string>
#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

/// Namespace dedicated to the AO* family of MDP algorithms.
namespace ao_search {

struct Statistics {
    unsigned long long iterations = 0;

    void print(std::ostream& out) const
    {
        out << "  Iterations: " << iterations << std::endl;
    }
};

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr uint8_t MARK = 1 << StateInfo::BITS;
    static constexpr uint8_t SOLVED = 2 << StateInfo::BITS;
    static constexpr uint8_t MASK = 3 << StateInfo::BITS;
    static constexpr uint8_t BITS = StateInfo::BITS + 2;

    bool is_tip_state() const { return update_order == 0; }
    bool is_marked() const { return this->info & MARK; }
    bool is_solved() const { return this->info & SOLVED; }
    bool is_unflagged() const { return (this->info & MASK) == 0; }

    void mark()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK) | MARK;
    }
    void unmark() { this->info = (this->info & ~MARK); }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }

    const std::vector<StateID>& get_parents() const { return parents; }
    std::vector<StateID>& get_parents() { return parents; }
    void add_parent(StateID s) { parents.push_back(s); }

    unsigned update_order = 0;
    std::vector<StateID> parents;
};

/**
 * @brief Base class for the AO* algorithm family.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam Interval - Determines whether interval bounds are used.
 * @tparam StorePolicy - Determines whether a greedy policy is maintained.
 * @tparam StateInfoExtension - The extended state information struct used by
 * the derived algorithm.
 */
template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
class AOBase
    : public heuristic_search::HeuristicSearchBase<
          State,
          Action,
          Interval,
          StorePolicy,
          StateInfoExtension> {
    /// The heuristic search base class.
    using HeuristicSearchBase = heuristic_search::HeuristicSearchBase<
        State,
        Action,
        Interval,
        StorePolicy,
        StateInfoExtension>;

    struct PrioritizedStateID {
        unsigned update_order;
        StateID state_id;

        friend bool operator<(
            const PrioritizedStateID& left,
            const PrioritizedStateID& right)
        {
            return left.update_order > right.update_order;
        }
    };

    std::priority_queue<PrioritizedStateID> queue_;

protected:
    using StateInfo = typename HeuristicSearchBase::StateInfo;

    std::vector<Action> aops_;
    Distribution<StateID> selected_transition_;

    Statistics statistics_;

public:
    AOBase(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<State, Action>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison)
        : HeuristicSearchBase(
              state_space,
              cost_function,
              value_init,
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison)
    {
    }

protected:
    void print_additional_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    void setup_custom_reports(param_type<State>) override
    {
        this->report_->register_print(
            [&](std::ostream& out) { out << "i=" << statistics_.iterations; });
    }

    void backpropagate_tip_value(utils::CountdownTimer& timer)
    {
        while (!queue_.empty()) {
            timer.throw_if_expired();

            auto elem = queue_.top();
            queue_.pop();

            auto& info = this->get_state_info(elem.state_id);
            assert(!info.is_goal_state());
            assert(!info.is_terminal() || info.is_solved());

            if (info.is_solved()) {
                // has been handled already
                continue;
            }

            assert(info.is_marked());
            info.unmark();

            bool solved = false;
            bool dead = false;
            bool value_changed =
                update_value_check_solved(elem.state_id, info, solved, dead);

            if (solved) {
                mark_solved_push_parents(elem.state_id, info, dead);
            } else if (value_changed) {
                push_parents_to_queue(info);
            }
        }
    }

    void backpropagate_update_order(StateID tip, utils::CountdownTimer& timer)
    {
        queue_.emplace(this->get_state_info(tip).update_order, tip);

        while (!queue_.empty()) {
            timer.throw_if_expired();

            auto elem = queue_.top();
            queue_.pop();

            auto& info = this->get_state_info(elem.state_id);
            if (info.update_order > elem.update_order) {
                continue;
            }

            std::erase_if(info.get_parents(), [this, elem](StateID state_id) {
                auto& pinfo = this->get_state_info(state_id);
                if (pinfo.is_solved()) {
                    return true;
                }

                if (pinfo.update_order <= elem.update_order) {
                    pinfo.update_order = elem.update_order + 1;
                    queue_.emplace(elem.update_order + 1, state_id);
                }

                return false;
            });
        }
    }

    void initialize_tip_state_value(
        StateID state,
        StateInfo& info,
        bool& terminal,
        bool& solved,
        bool& dead,
        bool& value_changed,
        utils::CountdownTimer& timer)
    {
        assert(!info.is_solved());
        assert(info.is_tip_state());
        assert(queue_.empty());

        terminal = false;
        solved = false;
        value_changed = update_value_check_solved(state, info, solved, dead);

        if (info.is_terminal()) {
            terminal = true;
            info.set_solved();
            dead = !info.is_goal_state();

            if (dead) {
                this->notify_dead_end(state);
            }

            push_parents_to_queue(info);
            backpropagate_tip_value(timer);
        }

        assert(queue_.empty());
    }

    void push_parents_to_queue(StateInfo& info)
    {
        auto& parents = info.get_parents();
        for (StateID parent : parents) {
            auto& pinfo = this->get_state_info(parent);
            assert(!pinfo.is_dead_end() || pinfo.is_solved());

            if constexpr (!StorePolicy) {
                if (info.is_solved()) {
                    assert(pinfo.unsolved > 0 || pinfo.is_solved());
                    --pinfo.unsolved;
                    if (!info.is_dead_end()) {
                        pinfo.alive = 1;
                    }
                }
            }

            if (pinfo.is_unflagged()) {
                pinfo.mark();
                queue_.emplace(pinfo.update_order, parent);
            }
        }

        if (info.is_solved()) {
            std::remove_reference_t<decltype(parents)>().swap(parents);
        }
    }

    void
    mark_solved_push_parents(StateID state, StateInfo& info, const bool dead)
    {
        assert(!info.is_terminal());

        if (dead) {
            assert(!info.is_solved() && !info.is_goal_state());
            this->notify_dead_end(state);
        }

        info.set_solved();
        push_parents_to_queue(info);
    }

private:
    bool update_value_check_solved(
        StateID state,
        const StateInfo& info,
        bool& solved,
        bool& dead)
    {
        if constexpr (StorePolicy) {
            ClearGuard guard(selected_transition_);

            const bool result =
                this->async_update(state, &selected_transition_).value_changed;
            solved = true;
            dead = !selected_transition_.empty() || info.is_dead_end();

            for (const StateID succ_id : selected_transition_.support()) {
                const auto& succ_info = this->get_state_info(succ_id);
                solved = solved && succ_info.is_solved();
                dead = dead && succ_info.is_dead_end();
            }

            return result;
        } else {
            const bool result = this->async_update(state);

            solved = info.unsolved == 0;
            dead = solved && info.alive == 0 && !info.is_goal_state();

            return result;
        }
    }
};

} // namespace ao_search
} // namespace engines
} // namespace probfd

#endif // __AO_SEARCH_H__