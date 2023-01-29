#ifndef MDPS_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H
#define MDPS_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H

#include "probfd/engines/heuristic_search_base.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

#ifndef NDEBUG
#define DMSG(x) // x
#else
#define DMSG(x)
#endif

namespace probfd {
namespace engines {

namespace heuristic_depth_first_search {

enum class BacktrackingUpdateType {
    Disabled,
    OnDemand,
    Single,
    UntilConvergence,
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
template <typename State, typename Action, bool Interval, bool Fret>
using HDFSBase = std::conditional_t<
    Fret,
    heuristic_search::HeuristicSearchBase<
        State,
        Action,
        Interval,
        true,
        heuristic_search::NoAdditionalStateData>,
    heuristic_search::HeuristicSearchBase<
        State,
        Action,
        Interval,
        true,
        internal::PerStateInformation>>;

} // namespace internal

template <typename State, typename Action, bool Interval, bool Fret>
class HeuristicDepthFirstSearch
    : public internal::HDFSBase<State, Action, Interval, Fret> {

    using Statistics = internal::Statistics;

    using HeuristicSearchBase =
        internal::HDFSBase<State, Action, Interval, Fret>;

    using StateInfo = typename HeuristicSearchBase::StateInfo;

    using AdditionalStateInfo = std::
        conditional_t<Fret, internal::StandalonePerStateInformation, StateInfo>;

public:
    HeuristicDepthFirstSearch(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::PolicyPicker<Action>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
        engine_interfaces::StateEvaluator<State>* value_init,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy,
        bool LabelSolved,
        bool ForwardUpdates,
        BacktrackingUpdateType BackwardUpdates,
        bool CutoffInconsistent,
        bool GreedyExploration,
        bool PerformValueIteration,
        bool ExpandTipStates)
        : HeuristicSearchBase(
              state_id_map,
              action_id_map,
              transition_generator,
              cost_function,
              policy_chooser,
              new_state_handler,
              value_init,
              report,
              interval_comparison,
              stable_policy)
        , LabelSolved(LabelSolved)
        , ForwardUpdates(ForwardUpdates)
        , BackwardUpdates(BackwardUpdates)
        , CutoffInconsistent(CutoffInconsistent)
        , GreedyExploration(GreedyExploration)
        , PerformValueIteration(PerformValueIteration)
        , ExpandTipStates(ExpandTipStates)
    {
        initialize_persistent_state_storage();
    }

    virtual void reset_solver_state() override
    {
        delete (this->state_flags_);
        state_flags_ = new storage::PerStateStorage<AdditionalStateInfo>();
    }

    virtual value_t solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID stateid = this->get_state_id(state);
        if (PerformValueIteration) {
            solve_with_vi_termination(stateid);
        } else {
            solve_without_vi_termination(stateid);
        }

        return this->get_value(state);
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        HeuristicSearchBase::print_statistics(out);
        statistics_.print(out);
    }

private:
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
        explicit ExpansionInfo(const StateID& state)
            : stateid(state)
        {
        }

        void set_successors(const Distribution<StateID>& t)
        {
            for (const StateID s : t.elements()) {
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
    void initialize_persistent_state_storage()
    {
        if constexpr (std::is_same_v<AdditionalStateInfo, StateInfo>) {
            state_flags_ = &this->get_state_info_store();
        } else {
            state_flags_ = new storage::PerStateStorage<AdditionalStateInfo>();
        }
    }

    void solve_with_vi_termination(const StateID& stateid)
    {
        bool terminate = false;
        do {
            if (!policy_exploration<true>(stateid)) {
                unsigned ups = statistics_.backtracking_updates;
                statistics_.convergence_value_iterations++;
                // std::cout << "VI#" <<
                // statistics_.convergence_value_iterations
                //           << " on " << visited.size() << " SCCs" <<
                //           std::endl;
                auto x =
                    value_iteration(visited_.begin(), visited_.end(), false);
                terminate = !x.first && !x.second;
                statistics_.convergence_updates +=
                    (statistics_.backtracking_updates - ups);
                statistics_.backtracking_updates = ups;
            }

            visited_.clear();
            statistics_.iterations++;
            this->report(stateid);
        } while (!terminate);
    }

    void solve_without_vi_termination(const StateID& stateid)
    {
        bool terminate = false;
        do {
            terminate = !policy_exploration<false>(stateid);
            statistics_.iterations++;
            this->report(stateid);
            assert(visited_.empty());
        } while (!terminate);
    }

    template <bool GetVisited>
    bool policy_exploration(const StateID& state)
    {
        assert(state_infos_.empty());
        {
            AdditionalStateInfo& pers_info = state_flags_->operator[](state);
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

        DMSG(std::cout << "Policy depth-first exploration starting from "
                       << state << std::endl;)

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

            DMSG(std::cout << "   " << einfo.stateid << ": "
                           << "..." << std::endl;)

            bool fully_explored = true;
            if (keep_expanding) {
                while (!einfo.successors.empty()) {
                    StateID succid = einfo.successors.back();
                    einfo.successors.pop_back();

                    DMSG(std::cout << einfo.stateid << " -> " << succid
                                   << (state_infos_[succid].status)
                                   << std::endl;)

                    AdditionalStateInfo& pers_succ_info =
                        state_flags_->operator[](succid);

                    if (!pers_succ_info.is_solved()) {
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

                                DMSG(std::cout << "  => is onstack"
                                               << std::endl;)

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

                            DMSG(std::cout
                                     << "  => status=" << succ_info.status
                                     << " | unsl=" << einfo.unsolved_successor
                                     << " | dead=" << einfo.dead << std::endl;)

                            if (GreedyExploration && einfo.unsolved_successor) {
                                keep_expanding = false;
                                break;
                            }
                        } else if (
                            succ_info.status == LocalStateInfo::ONSTACK) {
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
                    } else {
                        if (!this->is_marked_dead_end(succid)) {
                            einfo.dead = false;
                        }

                        einfo.leaf = false;
                    }
                }
            }

            if (fully_explored) {
                last_lowlink = sinfo.lowlink;
                last_unsolved_successors = einfo.unsolved_successor;
                last_dead = einfo.dead;
                last_value_changed = einfo.value_changed;
                last_leaf = einfo.leaf;

                DMSG(std::cout << "backtracking out of " << einfo.stateid
                               << std::endl;)

                if (BackwardUpdates == BacktrackingUpdateType::Single ||
                    (last_value_changed &&
                     BackwardUpdates == BacktrackingUpdateType::OnDemand)) {
                    statistics_.backtracking_updates++;
                    auto result =
                        this->async_update(einfo.stateid, nullptr, nullptr);
                    last_value_changed = result.first;
                    last_unsolved_successors =
                        last_unsolved_successors || result.second;
                }

                if (sinfo.index == sinfo.lowlink) {
                    last_leaf = false;
                    unsigned scc_size = 0;

                    DMSG(std::cout << "backtracking from [";)

                    auto end = stack_.begin();
                    do {
                        state_infos_[*end].status = LocalStateInfo::UNSOLVED;
                        DMSG(std::cout << " " << *end;)
                        ++scc_size;
                    } while (*(end++) != einfo.stateid);

                    DMSG(std::cout << " ] -> " << last_unsolved_successors
                                   << "|" << last_dead << std::endl;)

                    if constexpr (GetVisited) {
                        if (!last_unsolved_successors && !last_value_changed) {
                            visited_.reserve(visited_.size() + scc_size);
                            for (auto it = stack_.begin(); it != end; ++it) {
                                visited_.push_back(*it);
                            }
                        }
                    }

                    if (BackwardUpdates ==
                            BacktrackingUpdateType::UntilConvergence &&
                        last_unsolved_successors) {
                        auto result =
                            value_iteration(stack_.begin(), end, true);
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
                                state_flags_->operator[](*it).set_solved();
                                // std::cout << "marking " << (*it2) << "
                                // solved" << std::endl;
                            }
                        }
                    }

                    stack_.erase(stack_.begin(), end);
                }

                expansion_queue_.pop_back();
            }
        } while (!expansion_queue_.empty());

        state_infos_.clear();

        // std::cout << "====> POLICY FULLY EXPLORED: " <<
        // last_unsolved_successors
        // << std::endl;

        return last_unsolved_successors || last_value_changed;
    }

    uint8_t push(
        const StateID& stateid,
        AdditionalStateInfo& sinfo,
        bool& parent_value_changed,
        bool& parent_unsolved_successors)
    {
        assert(!sinfo.is_solved());

        const bool is_tip_state = !sinfo.is_policy_initialized();
        ExpansionInfo& einfo = expansion_queue_.emplace_back(stateid);

        if (ForwardUpdates || is_tip_state) {
            sinfo.set_policy_initialized();
            statistics_.forward_updates++;
            const bool updated =
                this->async_update(stateid, nullptr, &transition_).first;
            einfo.value_changed = updated;

            if constexpr (Interval) {
                parent_value_changed =
                    parent_value_changed || einfo.value_changed ||
                    (this->interval_comparison_ &&
                     !this->get_state_info(stateid, sinfo)
                          .value.bounds_approximately_equal());
            } else {
                parent_value_changed =
                    parent_value_changed || einfo.value_changed;
            }

            DMSG(std::cout << "TIP " << stateid << " -> " << einfo.value_changed
                           << std::endl;)

            if (transition_.empty()) {
                DMSG(std::cout << stateid << " is a leaf" << std::endl;)

                expansion_queue_.pop_back();
                sinfo.set_solved();
                uint8_t closed = LocalStateInfo::CLOSED;
                if (this->notify_dead_end_ifnot_goal(stateid)) {
                    closed = LocalStateInfo::CLOSED_DEAD;
                }

                return updated ? LocalStateInfo::UNSOLVED : closed;
            } else if (
                (!ExpandTipStates && is_tip_state) ||
                (CutoffInconsistent && einfo.value_changed)) {
                transition_.clear();
                expansion_queue_.pop_back();
                parent_unsolved_successors = true;

                return LocalStateInfo::UNSOLVED;
            }

            einfo.set_successors(transition_);
            transition_.clear();
        } else {
            if (this->get_state_info(stateid, sinfo).is_dead_end()) {
                sinfo.set_solved();
                return LocalStateInfo::CLOSED_DEAD;
            }

            this->apply_policy(stateid, transition_);
            einfo.set_successors(transition_);
            transition_.clear();
        }

        return LocalStateInfo::ONSTACK;
    }

    template <typename Iterator>
    std::pair<bool, bool>
    value_iteration(Iterator begin, Iterator end, bool until_convergence)
    {
        std::pair<bool, bool> updated_all(false, false);
        bool value_changed = true;
        bool policy_graph_changed;

        do {
            bool all_converged = true;

            value_changed = false;
            policy_graph_changed = false;

            for (auto it = begin; it != end; ++it) {
                StateID id = *it;

                statistics_.backtracking_updates++;

                DMSG(std::cout << "updating " << id << " "
                               << (get_state_info(id).get_value()) << " ... ";)

                const auto [val_change, policy_changed] =
                    this->async_update(id, nullptr, nullptr);
                value_changed = val_change || value_changed;

                if constexpr (Interval) {
                    all_converged = all_converged &&
                                    (!this->interval_comparison_ ||
                                     this->get_state_info(id)
                                         .value.bounds_approximately_equal());
                }

                policy_graph_changed = policy_graph_changed || policy_changed;

                DMSG(std::cout << (get_state_info(id).get_value()) << " ["
                               << valupd << "|" << policy_changed << std::endl;)
            }

            updated_all.first =
                updated_all.first || value_changed || !all_converged;
            updated_all.second = updated_all.second || policy_graph_changed;
        } while (value_changed && (until_convergence || !policy_graph_changed));

        return updated_all;
    }

    const bool LabelSolved;
    const bool ForwardUpdates;
    const BacktrackingUpdateType BackwardUpdates;
    const bool CutoffInconsistent;
    const bool GreedyExploration;
    const bool PerformValueIteration;
    const bool ExpandTipStates;

    storage::PerStateStorage<AdditionalStateInfo>* state_flags_;

    storage::StateHashMap<LocalStateInfo> state_infos_;
    std::vector<StateID> visited_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;
    Distribution<StateID> transition_;

    Statistics statistics_;
};

} // namespace heuristic_depth_first_search
} // namespace engines
} // namespace probfd

#undef DMSG

#endif // __HEURISTIC_DEPTH_FIRST_SEARCH_H__