#ifndef MDPS_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H
#define MDPS_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H

#include "heuristic_search_base.h"

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

namespace probabilistic {
namespace engines {

/// I do not know this algorithm.
namespace heuristic_depth_first_search {

enum class BacktrackingUpdateType {
    Disabled,
    OnDemand,
    Single,
    UntilConvergence,
};

namespace internal {

struct Statistics {

    unsigned long long iterations;
    unsigned long long forward_updates;
    unsigned long long backtracking_updates;
    unsigned long long convergence_updates;
    unsigned long long backtracking_value_iterations;
    unsigned long long convergence_value_iterations;

    Statistics()
        : iterations(0)
        , forward_updates(0)
        , backtracking_updates(0)
        , convergence_updates(0)
        , backtracking_value_iterations(0)
        , convergence_value_iterations(0)
    {
    }

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

    void register_report(ProgressReport* report)
    {
        report->register_print(
            [this](std::ostream& out) { out << "dohs=" << iterations; });
    }
};

struct PerStateInformationBase {
    static constexpr const uint8_t BITS = 0;
    PerStateInformationBase()
        : info(0)
    {
    }
    uint8_t info;
};

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr const uint8_t INITIALIZED = (1 << StateInfo::BITS);
    static constexpr const uint8_t SOLVED = (2 << StateInfo::BITS);
    static constexpr const uint8_t BITS = StateInfo::BITS + 2;
    static constexpr const uint8_t MASK = (3 << StateInfo::BITS);

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

template <
    typename State,
    typename Action,
    typename DualBounds,
    typename StorePolicy,
    template <typename>
    class StateInfoT,
    typename AdditionalPerStateInformation =
        typename heuristic_search::HeuristicSearchBase<
            State,
            Action,
            DualBounds,
            StorePolicy,
            StateInfoT>::StateInfo>
class HeuristicDepthFirstSearch
    : public heuristic_search::HeuristicSearchBase<
          State,
          Action,
          DualBounds,
          StorePolicy,
          StateInfoT> {

    using HeuristicSearchBase = heuristic_search::
        HeuristicSearchBase<State, Action, DualBounds, StorePolicy, StateInfoT>;

public:
    HeuristicDepthFirstSearch(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_utils::IntervalValue reward_bound,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        StateEvaluator<State>* dead_end_eval,
        DeadEndListener<State, Action>* dead_end_listener,
        PolicyPicker<Action>* policy_chooser,
        NewStateHandler<State>* new_state_handler,
        StateEvaluator<State>* value_init,
        HeuristicSearchConnector* connector,
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
              state_reward_function,
              action_reward_function,
              reward_bound,
              aops_generator,
              transition_generator,
              dead_end_eval,
              dead_end_listener,
              policy_chooser,
              new_state_handler,
              value_init,
              connector,
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
        , state_flags_(nullptr)
        , state_infos_()
        , visited_()
        , expansion_queue_()
        , stack_()
        , statistics_()
    {
        initialize_persistent_state_storage(
            std::is_same<
                AdditionalPerStateInformation,
                typename HeuristicSearchBase::StateInfo>());
    }

    ~HeuristicDepthFirstSearch()
    {
        if (!std::is_same<
                AdditionalPerStateInformation,
                HeuristicSearchBase::StateInfo>::value) {
            delete (this->state_flags_);
        }
    }

    virtual void reset_solver_state() override
    {
        delete (this->state_flags_);
        state_flags_ =
            new storage::PerStateStorage<AdditionalPerStateInformation>();
    }

    virtual void solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID stateid = this->get_state_id(state);
        if (PerformValueIteration) {
            solve_with_vi_termination(stateid);
        } else {
            solve_without_vi_termination(stateid);
        }
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

        explicit LocalStateInfo()
            : status(NEW)
            , index(UNDEF)
            , lowlink(UNDEF)
        {
        }

        uint8_t status;
        unsigned index;
        unsigned lowlink;

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
            for (auto it = t.begin(); it != t.end(); ++it) {
                successors.push_back(it->first);
            }
        }

        const StateID stateid;
        std::vector<StateID> successors;
        bool dead = true;
        bool unsolved_successor = false;
        bool value_changed = false;
        bool leaf = true;
    };

    void initialize_persistent_state_storage(const std::true_type&)
    {
        state_flags_ = &this->get_state_info_store();
    }

    void initialize_persistent_state_storage(const std::false_type&)
    {
        state_flags_ =
            new storage::PerStateStorage<AdditionalPerStateInformation>();
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
                    value_iteration<false>(visited_.begin(), visited_.end());
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
            AdditionalPerStateInformation& pers_info =
                state_flags_->operator[](state);
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
            sinfo.lowlink =
                std::min((const unsigned&)sinfo.lowlink, last_lowlink);
            einfo.unsolved_successor =
                einfo.unsolved_successor || last_unsolved_successors;
            einfo.value_changed = einfo.value_changed || last_value_changed;
            einfo.dead = einfo.dead && last_dead;
            einfo.leaf = einfo.leaf && last_leaf;

            DMSG(std::cout << "   " << einfo.stateid << ": "
                           << "..." << std::endl;)

            bool fully_explored = true;
            if (keep_expanding) {
                for (int i = einfo.successors.size() - 1; i >= 0; --i) {
                    StateID succid = einfo.successors.back();
                    einfo.successors.pop_back();
                    DMSG(std::cout << einfo.stateid << " -> " << succid
                                   << (state_infos_[succid].status)
                                   << std::endl;)
                    AdditionalPerStateInformation& pers_succ_info =
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
                            } else {
                                einfo.leaf = false;
                                succ_info.status = status;
                                if (status == LocalStateInfo::UNSOLVED) {
                                    einfo.unsolved_successor = true;
                                    einfo.dead = false;
                                } else if (status == LocalStateInfo::CLOSED) {
                                    einfo.dead = false;
                                }
                                DMSG(std::cout << "  => status="
                                               << succ_info.status << " | unsl="
                                               << einfo.unsolved_successor
                                               << " | dead=" << einfo.dead
                                               << std::endl;)
                                if (GreedyExploration &&
                                    einfo.unsolved_successor) {
                                    keep_expanding = false;
                                    break;
                                }
                            }
                        } else if (
                            succ_info.status == LocalStateInfo::ONSTACK) {
                            sinfo.lowlink = std::min(
                                (const unsigned&)sinfo.lowlink,
                                succ_info.index);
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
                        LocalStateInfo& sinfo = state_infos_[*end];
                        sinfo.status = LocalStateInfo::UNSOLVED;
                        DMSG(std::cout << " " << *end;)
                        ++scc_size;
                        if (*end == einfo.stateid) {
                            ++end;
                            break;
                        }
                        ++end;
                    } while (true);
                    DMSG(std::cout << " ] -> " << last_unsolved_successors
                                   << "|" << last_dead << std::endl;)

                    if (GetVisited &&
                        (!last_unsolved_successors && !last_value_changed)) {
                        visited_.reserve(visited_.size() + scc_size);
                        for (auto it = stack_.begin(); it != end; ++it) {
                            visited_.push_back(*it);
                        }
                    }

                    if (BackwardUpdates ==
                            BacktrackingUpdateType::UntilConvergence &&
                        last_unsolved_successors) {
                        auto result =
                            value_iteration<true>(stack_.begin(), end);
                        last_value_changed = result.first;
                        last_unsolved_successors =
                            result.second || last_unsolved_successors;
                    }

                    if (!this->is_dead_end_learning_enabled() ||
                        last_unsolved_successors ||
                        (scc_size != 1 && einfo.leaf)) {
                        last_dead = false;
                    }

                    last_unsolved_successors =
                        last_unsolved_successors || last_value_changed;

                    if (!last_unsolved_successors) {
                        const uint8_t closed = last_dead
                                                   ? LocalStateInfo::CLOSED_DEAD
                                                   : LocalStateInfo::CLOSED;
                        if (LabelSolved) {
                            for (auto it = stack_.begin(); it != end; ++it) {
                                state_infos_[*it].status = closed;
                                state_flags_->operator[](*it).set_solved();
                                // std::cout << "marking " << (*it2) << "
                                // solved" << std::endl;
                            }
                        } else {
                            for (auto it = stack_.begin(); it != end; ++it) {
                                state_infos_[*it].status = closed;
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
        AdditionalPerStateInformation& sinfo,
        bool& parent_value_changed,
        bool& parent_unsolved_successors)
    {
        assert(!sinfo.is_solved());
        const bool is_tip_state = !sinfo.is_policy_initialized();
        expansion_queue_.emplace_back(stateid);
        ExpansionInfo& einfo = expansion_queue_.back();
        if (ForwardUpdates || is_tip_state) {
            sinfo.set_policy_initialized();
            statistics_.forward_updates++;
            const bool updated =
                this->async_update(stateid, nullptr, &transition_).first;
            einfo.value_changed = updated;

            if constexpr (DualBounds::value) {
                parent_value_changed =
                    parent_value_changed || einfo.value_changed ||
                    (this->interval_comparison_ &&
                     !get_state_info(stateid, sinfo).value.bounds_equal());
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
            const auto& i = get_state_info(stateid, sinfo);
            if (i.is_dead_end()) {
                sinfo.set_solved();
                return LocalStateInfo::CLOSED_DEAD;
            } else {
                this->apply_policy(stateid, transition_);
                einfo.set_successors(transition_);
                transition_.clear();
            }
        }
        return LocalStateInfo::ONSTACK;
    }

    template <bool UntilConvergence, typename Iterator>
    std::pair<bool, bool> value_iteration(Iterator begin, Iterator end)
    {
        std::pair<bool, bool> updated_all(false, false);
        bool value_changed = true;
        do {
            value_changed = false;
            bool all_converged = true;
            bool policy_graph_changed = false;
            for (auto it = begin; it != end; ++it) {
                statistics_.backtracking_updates++;
                DMSG(StateID id = *it;
                     std::cout << "updating " << id << " "
                               << (get_state_info(id).get_value()) << " ... ";)
                auto result = this->async_update(*it, nullptr, nullptr);

                value_changed = result.first || value_changed;
                bool policy_changed = result.second;

                if constexpr (DualBounds::value) {
                    all_converged = all_converged &&
                                    (!this->interval_comparison_ ||
                                     get_state_info(*it).value.bounds_equal());
                }

                policy_graph_changed = policy_graph_changed || policy_changed;
                DMSG(std::cout << (get_state_info(id).get_value()) << " ["
                               << valupd << "|" << policy_changed << std::endl;)
            }
            updated_all.first =
                updated_all.first || value_changed || !all_converged;
            updated_all.second = updated_all.second || policy_graph_changed;
            if (!UntilConvergence && policy_graph_changed) {
                break;
            }
        } while (value_changed);
        return updated_all;
    }

    const bool LabelSolved;
    const bool ForwardUpdates;
    const BacktrackingUpdateType BackwardUpdates;
    const bool CutoffInconsistent;
    const bool GreedyExploration;
    const bool PerformValueIteration;
    const bool ExpandTipStates;

    storage::PerStateStorage<AdditionalPerStateInformation>* state_flags_;

    storage::StateHashMap<LocalStateInfo> state_infos_;
    std::vector<StateID> visited_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;
    Distribution<StateID> transition_;

    Statistics statistics_;
};

} // namespace internal

template <typename State, typename Action, typename B2, typename Fret>
class HeuristicDepthFirstSearch;

template <typename State, typename Action, typename B2>
class HeuristicDepthFirstSearch<State, Action, B2, std::false_type>
    : public internal::HeuristicDepthFirstSearch<
          State,
          Action,
          B2,
          std::true_type,
          internal::PerStateInformation> {
public:
    using internal::HeuristicDepthFirstSearch<
        State,
        Action,
        B2,
        std::true_type,
        internal::PerStateInformation>::HeuristicDepthFirstSearch;
};

template <typename State, typename Action, typename B2>
class HeuristicDepthFirstSearch<State, Action, B2, std::true_type>
    : public internal::HeuristicDepthFirstSearch<
          State,
          Action,
          B2,
          std::true_type,
          heuristic_search::NoAdditionalStateData,
          internal::StandalonePerStateInformation> {
public:
    using internal::HeuristicDepthFirstSearch<
        State,
        Action,
        B2,
        std::true_type,
        heuristic_search::NoAdditionalStateData,
        internal::StandalonePerStateInformation>::HeuristicDepthFirstSearch;
};

} // namespace heuristic_depth_first_search
} // namespace engines
} // namespace probabilistic

#undef DMSG

#endif // __HEURISTIC_DEPTH_FIRST_SEARCH_H__