#ifndef PROBFD_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H
#define PROBFD_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H

#include "probfd/engines/heuristic_search_base.h"

#include "utils/countdown_timer.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

namespace heuristic_depth_first_search {

enum class BacktrackingUpdateType {
    DISABLED,
    ON_DEMAND,
    SINGLE,
    CONVERGENCE,
};

namespace internal {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long forward_updates = 0;
    unsigned long long backtracking_updates = 0;
    unsigned long long convergence_updates = 0;
    unsigned long long backtracking_value_iterations = 0;
    unsigned long long convergence_value_iterations = 0;

    void print(std::ostream& out) const
    {
        out << "  Iterations: " << iterations << std::endl;
        out << "  Value iterations (backtracking): "
            << backtracking_value_iterations << std::endl;
        out << "  Value iterations (convergence): "
            << convergence_value_iterations << std::endl;
        out << "  Bellman backups (forward): " << forward_updates << std::endl;
        out << "  Bellman backups (backtracking): " << backtracking_updates
            << std::endl;
        out << "  Bellman backups (convergence): " << convergence_updates
            << std::endl;
    }
};

struct PerStateInformationBase {
    static constexpr uint8_t BITS = 0;
    uint8_t info = 0;
};

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr uint8_t INITIALIZED = 1 << StateInfo::BITS;
    static constexpr uint8_t SOLVED = 2 << StateInfo::BITS;
    static constexpr uint8_t MASK = 3 << StateInfo::BITS;
    static constexpr uint8_t BITS = StateInfo::BITS + 2;

    bool is_policy_initialized() const { return (this->info & MASK) != 0; }
    bool is_solved() const { return this->info & SOLVED; }
    void set_policy_initialized()
    {
        this->info = (this->info & ~MASK) | INITIALIZED;
    }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
};

using StandalonePerStateInformation =
    PerStateInformation<PerStateInformationBase>;

// Store HDFS-specific state information seperately when FRET is enabled to
// make resetting the solver state easier.
template <typename State, typename Action, bool UseInterval, bool Fret>
using HDFSBase = std::conditional_t<
    Fret,
    heuristic_search::HeuristicSearchBase<
        State,
        Action,
        UseInterval,
        true,
        heuristic_search::NoAdditionalStateData>,
    heuristic_search::HeuristicSearchBase<
        State,
        Action,
        UseInterval,
        true,
        internal::PerStateInformation>>;

} // namespace internal

/**
 * @brief Implementation of the depth-first heuristic search algorithm family
 * \cite steinmetz:etal:icaps-16.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 * @tparam Fret - Whether the algorithm is wrapped with the FRET framework.
 */
template <typename State, typename Action, bool UseInterval, bool Fret>
class HeuristicDepthFirstSearch
    : public internal::HDFSBase<State, Action, UseInterval, Fret> {

    using Statistics = internal::Statistics;

    using HeuristicSearchBase =
        internal::HDFSBase<State, Action, UseInterval, Fret>;

    using StateInfo = typename HeuristicSearchBase::StateInfo;

    using AdditionalStateInfo = std::
        conditional_t<Fret, internal::StandalonePerStateInformation, StateInfo>;

    struct LocalStateInfo {
        static constexpr const uint8_t NEW = 0;
        static constexpr const uint8_t ONSTACK = 1;
        static constexpr const uint8_t CLOSED = 2;
        static constexpr const uint8_t CLOSED_DEAD = 3;
        static constexpr const uint8_t UNSOLVED = 4;
        static constexpr const unsigned UNDEF = ((unsigned)-1) >> 1;

        uint8_t status = NEW;
        unsigned index = UNDEF;
        unsigned lowlink = UNDEF;

        void open(const unsigned& index)
        {
            status = ONSTACK;
            this->index = index;
            this->lowlink = index;
        }
    };

    struct ExpansionInfo {
        ExpansionInfo(StateID state, const Distribution<StateID>& t)
            : stateid(state)
        {
            for (const StateID s : t.support()) {
                successors.push_back(s);
            }
        }

        const StateID stateid;

        std::vector<StateID> successors;

        bool dead = true;
        bool unsolved_successor = false;
        bool value_changed = false;
        bool leaf = true;
    };

    const bool LabelSolved;
    const bool ForwardUpdates;
    const BacktrackingUpdateType BackwardUpdates;
    const bool CutoffInconsistent;
    const bool GreedyExploration;
    const bool PerformValueIteration;
    const bool ExpandTipStates;

    storage::PerStateStorage<AdditionalStateInfo> state_flags_;

    storage::StateHashMap<LocalStateInfo> state_infos_;
    std::vector<StateID> visited_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;
    Distribution<StateID> transition_;

    Statistics statistics_;

public:
    HeuristicDepthFirstSearch(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<State, Action>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        bool LabelSolved,
        bool ForwardUpdates,
        BacktrackingUpdateType BackwardUpdates,
        bool CutoffInconsistent,
        bool GreedyExploration,
        bool PerformValueIteration,
        bool ExpandTipStates)
        : HeuristicSearchBase(
              state_space,
              cost_function,
              value_init,
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison)
        , LabelSolved(LabelSolved)
        , ForwardUpdates(ForwardUpdates)
        , BackwardUpdates(BackwardUpdates)
        , CutoffInconsistent(CutoffInconsistent)
        , GreedyExploration(GreedyExploration)
        , PerformValueIteration(PerformValueIteration)
        , ExpandTipStates(ExpandTipStates)
    {
    }

    void reset_search_state() override { state_flags_.clear(); }

protected:
    Interval do_solve(param_type<State> state, double max_time) override
    {
        utils::CountdownTimer timer(max_time);

        const StateID stateid = this->get_state_id(state);
        if (PerformValueIteration) {
            solve_with_vi_termination(stateid, timer);
        } else {
            solve_without_vi_termination(stateid, timer);
        }

        return this->lookup_bounds(stateid);
    }

    void print_additional_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

private:
    AdditionalStateInfo& get_pers_info(StateID state_id)
    {
        if constexpr (Fret) {
            return state_flags_[state_id];
        } else {
            return this->get_state_info(state_id);
        }
    }

    void
    solve_with_vi_termination(StateID stateid, utils::CountdownTimer& timer)
    {
        bool terminate = false;
        do {
            if (!policy_exploration<true>(stateid, timer)) {
                unsigned ups = statistics_.backtracking_updates;
                statistics_.convergence_value_iterations++;
                auto x = value_iteration(visited_, false, timer);
                terminate = !x.first && !x.second;
                statistics_.convergence_updates +=
                    (statistics_.backtracking_updates - ups);
                statistics_.backtracking_updates = ups;
            }

            visited_.clear();
            statistics_.iterations++;
            this->print_progress();
        } while (!terminate);
    }

    void
    solve_without_vi_termination(StateID stateid, utils::CountdownTimer& timer)
    {
        bool terminate = false;
        do {
            terminate = !policy_exploration<false>(stateid, timer);
            statistics_.iterations++;
            this->print_progress();
            assert(visited_.empty());
        } while (!terminate);
    }

    template <bool GetVisited>
    bool policy_exploration(StateID state, utils::CountdownTimer& timer)
    {
        using enum BacktrackingUpdateType;

        ClearGuard _guard(state_infos_);

        {
            AdditionalStateInfo& pers_info = get_pers_info(state);
            bool value_changed = false;
            bool pruned = false;
            const uint8_t pstatus =
                push(state, pers_info, value_changed, pruned);

            if (pers_info.is_solved()) {
                // is terminal
                return false;
            }

            if (pstatus != LocalStateInfo::ONSTACK) {
                return value_changed || pruned;
            }

            stack_.push_back(state);
            state_infos_[state].open(0);
        }

        unsigned current_index = 0;
        bool keep_expanding = true;
        bool last_unsolved_successors = false;
        bool last_value_changed = false;
        bool last_dead = true;
        bool last_leaf = true;
        unsigned last_lowlink = LocalStateInfo::UNDEF;

        do {
            ExpansionInfo& einfo = expansion_queue_.back();
            LocalStateInfo& sinfo = state_infos_[einfo.stateid];
            sinfo.lowlink = std::min(sinfo.lowlink, last_lowlink);
            einfo.unsolved_successor =
                einfo.unsolved_successor || last_unsolved_successors;
            einfo.value_changed = einfo.value_changed || last_value_changed;
            einfo.dead = einfo.dead && last_dead;
            einfo.leaf = einfo.leaf && last_leaf;

            bool fully_explored = true;
            if (keep_expanding) {
                while (!einfo.successors.empty()) {
                    timer.throw_if_expired();

                    StateID succid = einfo.successors.back();
                    einfo.successors.pop_back();

                    AdditionalStateInfo& pers_succ_info = get_pers_info(succid);

                    if (pers_succ_info.is_solved()) {
                        if (!this->is_marked_dead_end(succid)) {
                            einfo.dead = false;
                        }

                        einfo.leaf = false;
                        continue;
                    }

                    LocalStateInfo& succ_info = state_infos_[succid];
                    if (succ_info.status == LocalStateInfo::NEW) {
                        const uint8_t status = push(
                            succid,
                            pers_succ_info,
                            einfo.value_changed,
                            einfo.unsolved_successor);

                        if (status == LocalStateInfo::ONSTACK) {
                            last_value_changed = false;
                            last_unsolved_successors = false;
                            last_dead = true;
                            last_leaf = true;
                            last_lowlink = LocalStateInfo::UNDEF;
                            stack_.push_front(succid);
                            succ_info.open(++current_index);
                            fully_explored = false;

                            break;
                        }

                        einfo.leaf = false;
                        succ_info.status = status;

                        if (status == LocalStateInfo::UNSOLVED) {
                            einfo.unsolved_successor = true;
                            einfo.dead = false;
                        } else if (status == LocalStateInfo::CLOSED) {
                            einfo.dead = false;
                        }

                        if (GreedyExploration && einfo.unsolved_successor) {
                            keep_expanding = false;
                            break;
                        }
                    } else if (succ_info.status == LocalStateInfo::ONSTACK) {
                        sinfo.lowlink =
                            std::min(sinfo.lowlink, succ_info.index);
                    } else {
                        einfo.unsolved_successor =
                            einfo.unsolved_successor ||
                            succ_info.status == LocalStateInfo::UNSOLVED;
                        einfo.leaf = false;
                        einfo.dead =
                            einfo.dead &&
                            succ_info.status == LocalStateInfo::CLOSED_DEAD;
                    }
                }
            }

            if (!fully_explored) {
                continue;
            }

            last_lowlink = sinfo.lowlink;
            last_unsolved_successors = einfo.unsolved_successor;
            last_dead = einfo.dead;
            last_value_changed = einfo.value_changed;
            last_leaf = einfo.leaf;

            if (BackwardUpdates == SINGLE ||
                (last_value_changed && BackwardUpdates == ON_DEMAND)) {
                statistics_.backtracking_updates++;
                auto result = this->async_update(einfo.stateid, nullptr);
                last_value_changed = result.value_changed;
                last_unsolved_successors =
                    last_unsolved_successors || result.policy_changed;
            }

            if (sinfo.index == sinfo.lowlink) {
                last_leaf = false;

                auto end = stack_.begin();
                do {
                    state_infos_[*end].status = LocalStateInfo::UNSOLVED;
                } while (*(end++) != einfo.stateid);

                if constexpr (GetVisited) {
                    if (!last_unsolved_successors && !last_value_changed) {
                        visited_.insert(visited_.end(), stack_.begin(), end);
                    }
                }

                if (BackwardUpdates == CONVERGENCE &&
                    last_unsolved_successors) {
                    auto result = value_iteration(
                        std::ranges::subrange(stack_.begin(), end),
                        true,
                        timer);
                    last_value_changed = result.first;
                    last_unsolved_successors =
                        result.second || last_unsolved_successors;
                }

                last_dead = false;

                last_unsolved_successors =
                    last_unsolved_successors || last_value_changed;

                if (!last_unsolved_successors) {
                    const uint8_t closed = last_dead
                                               ? LocalStateInfo::CLOSED_DEAD
                                               : LocalStateInfo::CLOSED;
                    for (auto it = stack_.begin(); it != end; ++it) {
                        state_infos_[*it].status = closed;

                        if (LabelSolved) {
                            get_pers_info(*it).set_solved();
                        }
                    }
                }

                stack_.erase(stack_.begin(), end);
            }

            expansion_queue_.pop_back();

            timer.throw_if_expired();
        } while (!expansion_queue_.empty());

        return last_unsolved_successors || last_value_changed;
    }

    uint8_t push(
        StateID stateid,
        AdditionalStateInfo& sinfo,
        bool& parent_value_changed,
        bool& parent_unsolved_successors)
    {
        assert(!sinfo.is_solved());

        ClearGuard _guard(transition_);

        const bool is_tip_state = !sinfo.is_policy_initialized();

        if (ForwardUpdates || is_tip_state) {
            sinfo.set_policy_initialized();
            statistics_.forward_updates++;
            const bool value_changed =
                this->async_update(stateid, &transition_).value_changed;

            if constexpr (UseInterval) {
                parent_value_changed =
                    parent_value_changed || value_changed ||
                    (this->interval_comparison_ &&
                     !this->get_state_info(stateid, sinfo)
                          .value.bounds_approximately_equal());
            } else {
                parent_value_changed = parent_value_changed || value_changed;
            }

            if (transition_.empty()) {
                sinfo.set_solved();
                uint8_t closed = LocalStateInfo::CLOSED;
                if (this->notify_dead_end_ifnot_goal(stateid)) {
                    closed = LocalStateInfo::CLOSED_DEAD;
                }

                return value_changed ? LocalStateInfo::UNSOLVED : closed;
            }

            if ((!ExpandTipStates && is_tip_state) ||
                (CutoffInconsistent && value_changed)) {
                parent_unsolved_successors = true;

                return LocalStateInfo::UNSOLVED;
            }

            auto& einfo = expansion_queue_.emplace_back(stateid, transition_);
            einfo.value_changed = value_changed;
        } else {
            if (this->get_state_info(stateid, sinfo).is_dead_end()) {
                sinfo.set_solved();
                return LocalStateInfo::CLOSED_DEAD;
            }

            this->apply_policy(stateid, transition_);
            expansion_queue_.emplace_back(stateid, transition_);
        }

        return LocalStateInfo::ONSTACK;
    }

    std::pair<bool, bool> value_iteration(
        const std::ranges::input_range auto& range,
        bool until_convergence,
        utils::CountdownTimer& timer)
    {
        std::pair<bool, bool> updated_all(false, false);
        bool value_changed = true;
        bool policy_graph_changed;

        do {
            bool all_converged = true;

            value_changed = false;
            policy_graph_changed = false;

            for (const StateID id : range) {
                timer.throw_if_expired();

                statistics_.backtracking_updates++;

                const auto result = this->async_update(id, nullptr);
                value_changed = value_changed || result.value_changed;

                if constexpr (UseInterval) {
                    all_converged = all_converged &&
                                    (!this->interval_comparison_ ||
                                     this->get_state_info(id)
                                         .value.bounds_approximately_equal());
                }

                policy_graph_changed =
                    policy_graph_changed || result.policy_changed;
            }

            updated_all.first =
                updated_all.first || value_changed || !all_converged;
            updated_all.second = updated_all.second || policy_graph_changed;
        } while (value_changed && (until_convergence || !policy_graph_changed));

        return updated_all;
    }
};

} // namespace heuristic_depth_first_search
} // namespace engines
} // namespace probfd

#endif // __HEURISTIC_DEPTH_FIRST_SEARCH_H__