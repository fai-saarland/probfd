#ifndef GUARD_INCLUDE_PROBFD_PREPROCESSING_END_COMPONENT_DECOMPOSITION_H
#error "This file should only be included from end_component_decomposition.h"
#endif

#include "probfd/utils/language.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>
#include <ranges>
#include <type_traits>

namespace probfd::preprocessing {

inline void ECDStatistics::print(std::ostream& out) const
{
    out << "  Terminal states: " << terminals << " (" << goals
        << " goal states, " << selfloops << " self loop states)" << std::endl;
    out << "  Singleton SCC(s): " << sccs1 << " (" << sccs1_dead << " dead)"
        << std::endl;
    out << "  Non-singleton SCC(s): " << sccsk << " (" << sccsk_dead << " dead)"
        << std::endl;
    out << "  Singleton end component(s): " << ec1 << std::endl;
    out << "  Non-singleton end component(s): " << eck << std::endl;
    out << "  End-component transitions: " << ec_transitions << std::endl;
    out << "  Recursive calls: " << recursions << std::endl;
    out << "  End component computation: " << time << std::endl;
}

template <typename State, typename Action>
bool EndComponentDecomposition<State, Action>::StateInfo::onstack() const
{
    return stackid < UNDEF;
}

template <typename State, typename Action>
auto EndComponentDecomposition<State, Action>::StateInfo::get_status() const
{
    return explored ? (onstack() ? ONSTACK : CLOSED) : NEW;
}

template <typename State, typename Action>
EndComponentDecomposition<State, Action>::ExpansionInfo::ExpansionInfo(
    unsigned stck,
    std::vector<Action> aops,
    std::vector<std::vector<StateID>> successors)
    : stck(stck)
    , lstck(stck)
    , nz_or_leaves_scc(false)
    , aops(std::move(aops))
    , successors(std::move(successors))
{
}

template <typename State, typename Action>
EndComponentDecomposition<State, Action>::ExpansionInfo::ExpansionInfo(
    unsigned stck,
    std::vector<Action> aops,
    std::vector<std::vector<StateID>> successors,
    MDP& mdp)
    : stck(stck)
    , lstck(stck)
    , nz_or_leaves_scc(mdp.get_action_cost(aops.back()) != 0_vt)
    , aops(std::move(aops))
    , successors(std::move(successors))
{
}

template <typename State, typename Action>
bool EndComponentDecomposition<State, Action>::ExpansionInfo::next_action(
    std::nullptr_t)
{
    assert(aops.size() == successors.size());
    aops.pop_back();
    successors.pop_back();
    nz_or_leaves_scc = false;
    return !aops.empty();
}

template <typename State, typename Action>
bool EndComponentDecomposition<State, Action>::ExpansionInfo::next_action(
    MDP& mdp)
{
    assert(aops.size() == successors.size());
    aops.pop_back();
    successors.pop_back();

    if (!aops.empty()) {
        nz_or_leaves_scc = mdp.get_action_cost(aops.back()) != 0_vt;
        return true;
    }

    return false;
}

template <typename State, typename Action>
bool EndComponentDecomposition<State, Action>::ExpansionInfo::next_successor()
{
    auto& succs = successors.back();
    succs.pop_back();
    return !succs.empty();
}

template <typename State, typename Action>
StateID
EndComponentDecomposition<State, Action>::ExpansionInfo::get_current_successor()
{
    return successors.back().back();
}

template <typename State, typename Action>
Action&
EndComponentDecomposition<State, Action>::ExpansionInfo::get_current_action()
{
    return aops.back();
}

template <typename State, typename Action>
struct EndComponentDecomposition<State, Action>::StackInfo {
    StateID stateid;

    // SCC successors for ECD recursion
    std::vector<Action> aops;
    std::vector<std::vector<StateID>> successors;

    explicit StackInfo(StateID sid)
        : stateid(sid)
        , successors(1)
    {
    }

    template <size_t i>
    friend auto& get(StackInfo& info)
    {
        if constexpr (i == 0) return info.stateid;
        if constexpr (i == 1) return info.aops;
    }

    template <size_t i>
    friend const auto& get(const StackInfo& info)
    {
        if constexpr (i == 0) return info.stateid;
        if constexpr (i == 1) return info.aops;
    }
};

template <typename State, typename Action>
EndComponentDecomposition<State, Action>::EndComponentDecomposition(
    bool expand_goals)
    : expand_goals_(expand_goals)
{
}

template <typename State, typename Action>
auto EndComponentDecomposition<State, Action>::build_quotient_system(
    MDP& mdp,
    const Evaluator* pruning_function,
    param_type<State> initial_state,
    double max_time) -> std::unique_ptr<QuotientSystem>
{
    utils::CountdownTimer timer(max_time);

    stats_ = ECDStatistics();
    auto sys = std::make_unique<QuotientSystem>(mdp);

    auto init_id = mdp.get_state_id(initial_state);

    if (push(init_id, state_infos_[init_id], mdp, pruning_function)) {
        find_and_decompose_sccs(*sys, 0, timer, mdp, pruning_function);
    }

    assert(stack_.empty());
    assert(expansion_queue_.empty());
    stats_.time.stop();

    return sys;
}

template <typename State, typename Action>
void EndComponentDecomposition<State, Action>::print_statistics(
    std::ostream& out) const
{
    stats_.print(out);
}

template <typename State, typename Action>
ECDStatistics EndComponentDecomposition<State, Action>::get_statistics() const
{
    return stats_;
}

template <typename State, typename Action>
bool EndComponentDecomposition<State, Action>::push(
    StateID state_id,
    StateInfo& state_info,
    MDP& mdp,
    const Evaluator* pruning_function)
{
    state_info.explored = 1;
    State state = mdp.get_state(state_id);

    const auto term = mdp.get_termination_info(state);

    if (term.is_goal_state()) {
        ++stats_.terminals;
        ++stats_.goals;

        if (!expand_goals_) {
            return false;
        }

        state_info.expandable_goal = 1;
    } else if (
        pruning_function != nullptr &&
        pruning_function->evaluate(state) == term.get_cost()) {
        ++stats_.terminals;
        return false;
    }

    std::vector<Action> aops;
    mdp.generate_applicable_actions(state, aops);

    if (aops.empty()) {
        if (expand_goals_ && state_info.expandable_goal) {
            state_info.expandable_goal = 0;
        } else {
            ++stats_.terminals;
        }

        return false;
    }

    std::vector<std::vector<StateID>> successors;
    successors.reserve(aops.size());

    unsigned non_loop_actions = 0;
    for (unsigned i = 0; i < aops.size(); ++i) {
        Distribution<StateID> transition;
        mdp.generate_action_transitions(state, aops[i], transition);

        std::vector<StateID> succ_ids;

        for (StateID succ_id : transition.support()) {
            if (succ_id != state_id) {
                succ_ids.push_back(succ_id);
            }
        }

        if (!succ_ids.empty()) {
            successors.emplace_back(std::move(succ_ids));

            if (i != non_loop_actions) {
                aops[non_loop_actions] = aops[i];
            }

            ++non_loop_actions;
        }
    }

    // only self-loops
    if (non_loop_actions == 0) {
        if (expand_goals_ && state_info.expandable_goal) {
            state_info.expandable_goal = 0;
        } else {
            ++stats_.terminals;
            ++stats_.selfloops;
        }

        return false;
    }

    aops.erase(aops.begin() + non_loop_actions, aops.end());

    expansion_queue_.emplace_back(
        stack_.size(),
        std::move(aops),
        std::move(successors),
        mdp);

    state_info.stackid = stack_.size();
    stack_.emplace_back(state_id);

    return true;
}

template <typename State, typename Action>
bool EndComponentDecomposition<State, Action>::push(
    StateID state_id,
    StateInfo& info)
{
    assert(!info.explored);
    assert(info.onstack());

    info.explored = true;
    StackInfo& scc_info = stack_[info.stackid];

    if (scc_info.successors.empty()) {
        info.stackid = StateInfo::UNDEF;
        ++stats_.ec1;
        return false;
    }

    const auto stack_size = stack_.size();
    info.stackid = stack_size;

    expansion_queue_.emplace_back(
        stack_size,
        std::move(scc_info.aops),
        std::move(scc_info.successors));

    stack_.emplace_back(state_id);

    return true;
}

template <typename State, typename Action>
void EndComponentDecomposition<State, Action>::find_and_decompose_sccs(
    QuotientSystem& sys,
    const unsigned limit,
    utils::CountdownTimer& timer,
    auto&... mdp_and_h)
{
    if (expansion_queue_.size() <= limit) {
        return;
    }

    ExpansionInfo* e = &expansion_queue_.back();
    StackInfo* s = &stack_[e->stck];

    for (;;) {
        // DFS recursion
        while (push_successor(*e, *s, timer, mdp_and_h...)) {
            e = &expansion_queue_.back();
            s = &stack_[e->stck];
        }

        // Iterative backtracking
        do {
            assert(s->successors.back().empty());
            s->successors.pop_back();

            assert(e->successors.empty() && e->aops.empty());

            const bool recurse = e->recurse;
            const unsigned int stck = e->stck;
            const unsigned int lstck = e->lstck;

            assert(stck >= lstck);

            const bool scc_root = stck == lstck;

            if (scc_root) {
                scc_found<sizeof...(mdp_and_h) != 0>(sys, *e, *s, timer);
            }

            expansion_queue_.pop_back();

            if (expansion_queue_.size() <= limit) {
                return;
            }

            timer.throw_if_expired();

            e = &expansion_queue_.back();
            s = &stack_[e->stck];

            auto& stk_successors = s->successors.back();

            // Backtracked from successor.
            if (scc_root) { // Child SCC
                e->recurse = e->recurse || !stk_successors.empty();
                e->nz_or_leaves_scc = true;
            } else { // Same SCC
                e->lstck = std::min(e->lstck, lstck);

                e->recurse = e->recurse || recurse || e->nz_or_leaves_scc;
                stk_successors.emplace_back(e->get_current_successor());
            }

            // If a successor exists stop backtracking
            if (e->next_successor()) {
                break;
            }

            // Finalize fully explored transition.
            if (!e->nz_or_leaves_scc) {
                assert(!stk_successors.empty());
                s->aops.push_back(e->get_current_action());
                s->successors.emplace_back();
            } else {
                stk_successors.clear();
            }
        } while (!e->next_action(select_opt<0>(mdp_and_h...)));
    }
}

template <typename State, typename Action>
bool EndComponentDecomposition<State, Action>::push_successor(
    ExpansionInfo& e,
    StackInfo& s,
    utils::CountdownTimer& timer,
    auto&... mdp_and_h)
{
    do {
        std::vector<StateID>& s_succs = s.successors.back();

        do {
            timer.throw_if_expired();

            const StateID succ_id = e.get_current_successor();
            StateInfo& succ_info = state_infos_[succ_id];

            switch (succ_info.get_status()) {
            case StateInfo::NEW:
                if (push(succ_id, succ_info, mdp_and_h...)) {
                    return true;
                }

                [[fallthrough]];

            case StateInfo::CLOSED: // Child SCC
                e.recurse = e.recurse || !s_succs.empty();
                e.nz_or_leaves_scc = true;
                break;

            case StateInfo::ONSTACK: // Same SCC
                e.lstck = std::min(e.lstck, succ_info.stackid);

                e.recurse = e.recurse || e.nz_or_leaves_scc;
                s_succs.emplace_back(succ_id);
            }
        } while (e.next_successor());

        // Finalize fully explored transition.
        if (!e.nz_or_leaves_scc) {
            assert(!s_succs.empty());
            s.successors.emplace_back();
            s.aops.push_back(e.get_current_action());
        } else {
            s_succs.clear();
        }
    } while (e.next_action(select_opt<0>(mdp_and_h...)));

    return false;
}

template <typename State, typename Action>
template <bool RootIteration>
void EndComponentDecomposition<State, Action>::scc_found(
    QuotientSystem& sys,
    ExpansionInfo& e,
    StackInfo& s,
    utils::CountdownTimer& timer)
{
    auto scc = stack_ | std::views::drop(e.stck);

    if (scc.size()) {
        assert(s.aops.empty());
        const StateID scc_repr_id = s.stateid;
        StateInfo& info = state_infos_[scc_repr_id];
        info.stackid = StateInfo::UNDEF;

        stack_.pop_back();

        // Update stats
        ++stats_.ec1;

        if constexpr (RootIteration) {
            ++stats_.sccs1;
        }
    } else {
        if (expand_goals_) {
            for (auto& stk_info : scc) {
                assert(stk_info.successors.size() == stk_info.aops.size());
                StateInfo& info = state_infos_[stk_info.stateid];
                if (info.expandable_goal) {
                    stk_info.successors.clear();
                    stk_info.aops.clear();
                    e.recurse = true;
                }
            }
        }

        if (e.recurse) {
            ++stats_.recursions;

            if constexpr (RootIteration) {
                ++stats_.sccsk;
            }

            for (const auto& stk_info : scc) {
                assert(stk_info.successors.size() == stk_info.aops.size());
                state_infos_[stk_info.stateid].explored = 0;
            }

            decompose(sys, e.stck, timer);
        } else {
            unsigned transitions = 0;

            for (const auto& stk_info : scc) {
                assert(stk_info.successors.size() == stk_info.aops.size());
                StateInfo& info = state_infos_[stk_info.stateid];
                info.stackid = StateInfo::UNDEF;

                transitions += stk_info.aops.size();
            }

            sys.build_new_quotient(scc, s);
            stack_.erase(scc.begin(), scc.end());

            // Update stats
            ++stats_.eck;
            stats_.ec_transitions += transitions;

            if constexpr (RootIteration) {
                ++stats_.sccsk;
            }
        }
    }

    assert(stack_.size() == e.stck);
}

template <typename State, typename Action>
void EndComponentDecomposition<State, Action>::decompose(
    QuotientSystem& sys,
    unsigned start,
    utils::CountdownTimer& timer)
{
    const unsigned limit = expansion_queue_.size();

    for (unsigned i = start; i < stack_.size(); ++i) {
        const StateID id = stack_[i].stateid;
        StateInfo& state_info = state_infos_[id];

        if (!state_info.explored && push(id, state_info)) {
            // Recursively run decomposition
            find_and_decompose_sccs(sys, limit, timer);
        }
    }

    stack_.erase(stack_.begin() + start, stack_.end());

    assert(expansion_queue_.size() == limit);
}

} // namespace probfd::preprocessing
