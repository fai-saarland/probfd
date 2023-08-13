#ifndef GUARD_INCLUDE_PROBFD_ENGINES_AO_SEARCH_H
#error "This file should only be included from ao_search.h"
#endif

#include "downward/utils/countdown_timer.h"

namespace probfd {
namespace engines {
namespace ao_search {

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
void AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    print_additional_statistics(std::ostream& out) const
{
    statistics_.print(out);
}

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
void AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    setup_custom_reports(param_type<State>)
{
    this->report_->register_print(
        [&](std::ostream& out) { out << "i=" << statistics_.iterations; });
}

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
void AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    backpropagate_tip_value(
        MDP& mdp,
        Evaluator& heuristic,
        utils::CountdownTimer& timer)
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
        bool value_changed = update_value_check_solved(
            mdp,
            heuristic,
            elem.state_id,
            info,
            solved,
            dead);

        if (solved) {
            mark_solved_push_parents(info, dead);
        } else if (value_changed) {
            push_parents_to_queue(info);
        }
    }
}

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
void AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    backpropagate_update_order(StateID tip, utils::CountdownTimer& timer)
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

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
void AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    initialize_tip_state_value(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state,
        StateInfo& info,
        bool& terminal,
        bool& solved,
        bool& dead,
        bool& value_changed,
        utils::CountdownTimer& timer)
{
    assert(!info.is_solved() && info.is_tip_state());
    assert(queue_.empty());

    terminal = false;
    solved = false;
    value_changed =
        update_value_check_solved(mdp, heuristic, state, info, solved, dead);

    if (info.is_terminal()) {
        terminal = true;
        info.set_solved();
        dead = !info.is_goal_state();

        push_parents_to_queue(info);
        backpropagate_tip_value(mdp, heuristic, timer);
    }

    assert(queue_.empty());
}

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
void AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    push_parents_to_queue(StateInfo& info)
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
        utils::release_vector_memory(parents);
    }
}

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
void AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    mark_solved_push_parents(StateInfo& info, bool dead)
{
    assert(!info.is_terminal());

    if (dead) {
        assert(!info.is_solved() && !info.is_goal_state());
        this->notify_dead_end(info);
    }

    info.set_solved();
    push_parents_to_queue(info);
}

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
bool AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    update_value_check_solved(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state,
        const StateInfo& info,
        bool& solved,
        bool& dead)
    requires(StorePolicy)
{
    const auto update_result =
        this->bellman_policy_update(mdp, heuristic, state);
    const auto& greedy_transition = update_result.greedy_transition;

    solved = true;

    if (!greedy_transition) {
        dead = true;
        return update_result.value_changed;
    }

    dead = info.is_dead_end();

    for (const auto succ_id : greedy_transition->successor_dist.support()) {
        const auto& succ_info = this->get_state_info(succ_id);
        solved = solved && succ_info.is_solved();
        dead = dead && succ_info.is_dead_end();
    }

    return update_result.value_changed;
}

template <
    typename State,
    typename Action,
    bool Interval,
    bool StorePolicy,
    template <typename>
    class StateInfoExtension>
bool AOBase<State, Action, Interval, StorePolicy, StateInfoExtension>::
    update_value_check_solved(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state,
        const StateInfo& info,
        bool& solved,
        bool& dead)
    requires(!StorePolicy)
{
    const bool result = this->bellman_update(mdp, heuristic, state);

    solved = !info.unsolved;
    dead = solved && !info.alive && !info.is_goal_state();

    return result;
}

} // namespace ao_search
} // namespace engines
} // namespace probfd