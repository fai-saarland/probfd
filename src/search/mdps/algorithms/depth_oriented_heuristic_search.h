#pragma once

#include "../interfaces/i_printable.h"
#include "../progress_report.h"
#include "heuristic_search_base.h"
#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"

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
namespace algorithms {
namespace depth_oriented_search {

enum class BacktrackingUpdateType {
    Disabled,
    OnDemand,
    Single,
    UntilConvergence,
};

namespace internal {

struct Statistics : public IPrintable {

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

    virtual void print(std::ostream& out) const override
    {
        out << "**** Depth-Oriented Heuristic Search ****" << std::endl;
        out << "Iterations: " << iterations << std::endl;
        out << "Value iterations (backtracking): "
            << backtracking_value_iterations << std::endl;
        out << "Value iterations (convergence): "
            << convergence_value_iterations << std::endl;
        out << "Bellman backups (forward): " << forward_updates << std::endl;
        out << "Bellman backups (backtracking): " << backtracking_updates
            << std::endl;
        out << "Bellman backups (convergence): " << convergence_updates
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

template<typename StateInfo>
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

template<
    typename HeuristicSearchBase,
    typename PersistentStateInfo = typename HeuristicSearchBase::StateInfo>
class DepthOrientedHeuristicSearch : public HeuristicSearchBase {
public:
    using State = typename HeuristicSearchBase::State;
    using Action = typename HeuristicSearchBase::Action;
    using DualBounds = typename HeuristicSearchBase::DualBounds;

public:
    template<typename... Args>
    DepthOrientedHeuristicSearch(
        bool ForwardUpdates,
        BacktrackingUpdateType BackwardUpdates,
        bool CutoffInconsistent,
        bool GreedyExploration,
        bool PerformValueIteration,
        bool ExpandTipStates,
        StateListener<State, Action>* dead_end_listener,
        DeadEndIdentificationLevel level,
        Args... args)
        : HeuristicSearchBase(args...)
        , ForwardUpdates(ForwardUpdates)
        , BackwardUpdates(BackwardUpdates)
        , CutoffInconsistent(CutoffInconsistent)
        , GreedyExploration(GreedyExploration)
        , PerformValueIteration(PerformValueIteration)
        , ExpandTipStates(ExpandTipStates)
        , state_flags_(nullptr)
        , state_status_(
              this->template get_state_status_access<PersistentStateInfo>())
        , dead_end_listener_(dead_end_listener)
        , dead_end_ident_level_(level)
        , expansion_condition_(
              this->state_id_map_,
              this->state_flags_,
              this->state_status_,
              &this->state_infos_,
              level)
        , state_infos_()
        , visited_()
        , policy_()
        , expansion_queue_()
        , stack_()
        , statistics_()
    {
        initialize_persistent_state_storage(
            std::is_same<
                PersistentStateInfo,
                typename HeuristicSearchBase::StateInfo>());
        expansion_condition_.state_flags_ = state_flags_;
    }

    ~DepthOrientedHeuristicSearch()
    {
        if (!std::is_same<
                PersistentStateInfo,
                typename HeuristicSearchBase::StateInfo>::value) {
            delete (this->state_flags_);
        }
    }

    virtual void reset_solver_state() override
    {
        delete (this->state_flags_);
        state_flags_ = new storage::PerStateStorage<PersistentStateInfo>();
        expansion_condition_.state_flags_ = state_flags_;
    }

    virtual AnalysisResult solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID stateid = this->state_id_map_->operator[](state);
        bool terminate = false;
        do {
            if (PerformValueIteration) {
                if (!policy_exploration<true>(state)) {
                    unsigned ups = statistics_.backtracking_updates;
                    statistics_.convergence_value_iterations++;
                    // std::cout << "VI#" <<
                    // statistics_.convergence_value_iterations
                    //           << " on " << visited.size() << " SCCs" <<
                    //           std::endl;
                    terminate = !value_iteration<false>(0);
                    statistics_.convergence_updates +=
                        (statistics_.backtracking_updates - ups);
                    statistics_.backtracking_updates = ups;
                }
                visited_.clear();
                policy_.clear();
            } else {
                policy_exploration<false>(state);
                terminate = state_flags_->operator[](stateid).is_solved();
            }
            this->report(stateid);
            statistics_.iterations++;
            assert(visited_.empty() && policy_.empty());
        } while (!terminate);
        return this->create_result(state, new Statistics(statistics_));
    }

private:
    struct LocalStateInfo {
        static constexpr const uint8_t NEW = 0;
        static constexpr const uint8_t ONSTACK = 1;
        static constexpr const uint8_t CLOSED = 2;
        static constexpr const uint8_t CLOSED_DEAD = 3;
        static constexpr const uint8_t UNSOLVED = 4;
        static constexpr const unsigned UNDEF = ((unsigned)-1) >> 1;

        LocalStateInfo()
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

    struct ExpansionCondition {
        explicit ExpansionCondition(
            StateIDMap<State>* state_id_map,
            storage::PerStateStorage<PersistentStateInfo>* state_flags,
            typename HeuristicSearchBase::template StateStatusAccessor<
                PersistentStateInfo>& state_status,
            storage::StateHashMap<LocalStateInfo>* state_infos,
            const DeadEndIdentificationLevel level)
            : state_id_map_(state_id_map)
            , state_flags_(state_flags)
            , state_status_(state_status)
            , state_infos_(state_infos)
            , level_(level)
        {
        }

        bool operator()(const State& state)
        {
            StateID stateid = state_id_map_->operator[](state);
            auto& dohs_info = state_flags_->operator[](stateid);
            if (dohs_info.is_solved()) {
                return true;
            }
            switch (level_) {
            case (DeadEndIdentificationLevel::Policy): {
                return !state_infos_->contains(stateid);
            }
            case (DeadEndIdentificationLevel::Visited): {
                return !state_status_(stateid, dohs_info)
                            .is_value_initialized();
            }
            case (DeadEndIdentificationLevel::Complete):
                return false;
            default:
                break;
            }
            assert(false);
            return false;
        }

        StateIDMap<State>* state_id_map_;
        storage::PerStateStorage<PersistentStateInfo>* state_flags_;
        typename HeuristicSearchBase::template StateStatusAccessor<
            PersistentStateInfo>
            state_status_;
        storage::StateHashMap<LocalStateInfo>* state_infos_;
        const DeadEndIdentificationLevel level_;
    };

    struct ExpansionInfo {
        ExpansionInfo(const StateID& state)
            : stateid(state)
            , unsolved_successor(false)
            , updated(false)
            , dead(true)
            , flag(false)
        {
        }

        void
        set_successors(const Distribution<State>& t, StateIDMap<State>* id_map)
        {
            for (auto it = t.begin(); it != t.end(); ++it) {
                successors.push_back(id_map->operator[](it->first));
            }
        }

        const StateID stateid;
        std::vector<StateID> successors;
        bool unsolved_successor;
        bool updated;
        bool dead;
        bool flag;
        bool leaf = true;
    };

    void initialize_persistent_state_storage(const std::true_type&)
    {
        state_flags_ = &this->get_state_info_store();
    }

    void initialize_persistent_state_storage(const std::false_type&)
    {
        state_flags_ = new storage::PerStateStorage<PersistentStateInfo>();
    }

    template<bool GetVisited>
    bool policy_exploration(const State& initial_state)
    {
        assert(state_infos_.empty());
        bool last_unsolved_successors = false;
        {
            StateID sid = this->state_id_map_->operator[](initial_state);
            PersistentStateInfo& pers_info = state_flags_->operator[](sid);
            bool flag = false;
            const uint8_t pstatus =
                push(sid, pers_info, last_unsolved_successors, flag);
            if (pers_info.is_solved()) {
                // is terminal
                return false;
            }
            if (pstatus != LocalStateInfo::ONSTACK) {
                return pstatus == LocalStateInfo::UNSOLVED
                    || last_unsolved_successors;
            }
            stack_.push_back(sid);
            state_infos_[sid].open(0);
        }

        // std::cout << "----> " << initial_state << std::endl;
        // std::cout << "Policy depth-first exploration starting from " <<
        // initial_state << std::endl;
        unsigned current_index = 0;
        bool keep_expanding = true;
        bool last_dead = true;
        bool last_flag = false;
        bool last_leaf = true;
        unsigned last_lowlink = LocalStateInfo::UNDEF;
        do {
            ExpansionInfo& einfo = expansion_queue_.back();
            LocalStateInfo& sinfo = state_infos_[einfo.stateid];
            sinfo.lowlink =
                std::min((const unsigned&)sinfo.lowlink, last_lowlink);
            einfo.unsolved_successor =
                einfo.unsolved_successor || last_unsolved_successors;
            einfo.flag = einfo.flag || last_flag;
            einfo.dead = einfo.dead && last_dead;
            einfo.leaf = einfo.leaf && last_leaf;

            bool fully_explored = true;
            if (keep_expanding) {
                for (int i = einfo.successors.size() - 1; i >= 0; --i) {
                    StateID succid = einfo.successors.back();
                    einfo.successors.pop_back();
                    // std::cout << einfo.state << " -> " << succ <<
                    // (state_infos[succ].status) << std::endl;
                    PersistentStateInfo& pers_succ_info =
                        state_flags_->operator[](succid);
                    if (!pers_succ_info.is_solved()) {
                        LocalStateInfo& succ_info = state_infos_[succid];
                        if (succ_info.status == LocalStateInfo::NEW) {
                            const uint8_t status = push(
                                succid,
                                pers_succ_info,
                                einfo.unsolved_successor,
                                einfo.flag);
                            if (status == LocalStateInfo::ONSTACK) {
                                last_flag = false;
                                last_unsolved_successors = false;
                                last_lowlink = LocalStateInfo::UNDEF;
                                last_dead = true;
                                stack_.push_front(succid);
                                succ_info.open(++current_index);
                                fully_explored = false;
                                last_leaf = true;
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
                                if (GreedyExploration
                                    && einfo.unsolved_successor) {
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
                            einfo.unsolved_successor = einfo.unsolved_successor
                                || succ_info.status == LocalStateInfo::UNSOLVED;
                            einfo.leaf = false;
                            einfo.dead = einfo.dead
                                && succ_info.status
                                    == LocalStateInfo::CLOSED_DEAD;
                        }
                    } else {
                        if (dead_end_listener_ != nullptr
                            && !state_status_(succid, pers_succ_info)
                                    .is_dead_end()) {
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
                last_flag = einfo.flag;
                last_leaf = einfo.leaf;

                // std::cout << " <- " << einfo.state << std::endl;
                // std::cout << "backtracking out of " << einfo.state <<
                // std::endl;
                if (BackwardUpdates == BacktrackingUpdateType::Single
                    || (last_unsolved_successors
                        && BackwardUpdates
                            == BacktrackingUpdateType::OnDemand)) {
                    statistics_.backtracking_updates++;
                    State state =
                        this->state_id_map_->operator[](einfo.stateid);
                    const auto& i = state_status_(einfo.stateid);
                    Action pold = i.policy;
                    // std::cout << "backtracking update: "
                    //     << value_update_[einfo.state] << ", " << p << " -> ";
                    last_unsolved_successors = this->async_update(state)
                        || last_unsolved_successors || pold != i.policy;
                    last_flag = last_flag || last_unsolved_successors
                        || (DualBounds::value && this->interval_comparison_
                            && !i.bounds_equal());
                    // std::cout << "updated " << einfo.state << " -> "
                    //           << (state_status_->operator[](einfo.state)
                    //           AO_FAMILY        .get_value())
                    //           << "|" << last_unsolved_successors <<
                    //           std::endl;
                }

                if (sinfo.index == sinfo.lowlink) {
                    last_leaf = false;
                    unsigned scc_size = 0;
                    // std::cout << "backtracking from [";
                    auto end = stack_.begin();
                    do {
                        LocalStateInfo& sinfo = state_infos_[*end];
                        sinfo.status = LocalStateInfo::UNSOLVED;
                        // std::cout << " " << *it;
                        ++scc_size;
                        if (*end == einfo.stateid) {
                            ++end;
                            break;
                        }
                        ++end;
                    } while (true);
                    // std::cout << " ] -> " << last_unsolved_successors << "|"
                    //           << last_dead << std::endl;

                    if (BackwardUpdates
                            == BacktrackingUpdateType::UntilConvergence
                        || (GetVisited && !last_unsolved_successors)) {
                        const int prev_visited = visited_.size();
                        policy_.reserve(policy_.size() + scc_size);
                        visited_.reserve(visited_.size() + scc_size);
                        for (auto it = stack_.begin(); it != end; ++it) {
                            visited_.push_back(
                                this->state_id_map_->operator[](*it));
                            policy_.push_back(this->state_status_(*it).policy);
                        }
                        if (BackwardUpdates
                            == BacktrackingUpdateType::UntilConvergence) {
                            statistics_.backtracking_value_iterations++;
                            last_unsolved_successors =
                                value_iteration<true>(prev_visited)
                                || last_unsolved_successors;
                        }
                    }
                    if (dead_end_listener_ != nullptr
                        && !last_unsolved_successors && last_dead
                        && (scc_size == 1 || !einfo.leaf)) {
                        // std::cout << "-> checking for dead ends! <-" <<
                        // std::endl;
                        std::pair<bool, bool> updated;
                        State state =
                            this->state_id_map_->operator[](einfo.stateid);
                        // std::cout <<  state_status_(einfo.stateid).get_value() << std::endl;
                        // assert(this->has_dead_end_value(state));
                        this->safe_async_update(
                            state,
                            *dead_end_listener_,
                            expansion_condition_,
                            updated);
                        last_unsolved_successors = updated.second;
                        last_dead = !updated.first;
                        // std::cout << "------> result: " <<
                        // last_unsolved_successors << "|" << last_dead <<
                        // std::endl;
                    } else {
                        last_dead = false;
                    }
                    const uint8_t closed = last_dead
                        ? LocalStateInfo::CLOSED_DEAD
                        : LocalStateInfo::CLOSED;
                    if (PerformValueIteration) {
                        if (!last_unsolved_successors) {
                            for (auto it = stack_.begin(); it != end; ++it) {
                                state_infos_[*it].status = closed;
                            }
                        }
                    } else {
                        if (!last_unsolved_successors && !last_flag) {
                            for (auto it = stack_.begin(); it != end; ++it) {
                                state_infos_[*it].status = closed;
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
        return last_unsolved_successors;
    }

    uint8_t push(
        const StateID& stateid,
        PersistentStateInfo& sinfo,
        bool& unsolved,
        bool& flag)
    {
        assert(!sinfo.is_solved());
        const bool is_tip_state = !sinfo.is_policy_initialized();
        State state = this->state_id_map_->operator[](stateid);
        expansion_queue_.emplace_back(stateid);
        ExpansionInfo& einfo = expansion_queue_.back();
        if (ForwardUpdates || is_tip_state) {
            sinfo.set_policy_initialized();
            statistics_.forward_updates++;
            const bool updated =
                this->async_update(state, nullptr, &transition_);
            einfo.updated = updated;
            unsolved = einfo.updated || unsolved;
            flag = unsolved
                || (DualBounds::value && this->interval_comparison_
                    && !state_status_(stateid, sinfo).bounds_equal());
            // std::cout << "TIP " << state << " -> " << updated << "|"
            //           <<
            //           (state_status_->operator[](state).is_goal_state())
            //           << "|"
            //           << (state_status_->operator[](state).is_dead_end())
            //           << std::endl;
            if (transition_.empty()) {
                // std::cout << state << " is a leaf (" << value_update_[state]
                // <<
                // ")" << std::endl;
                expansion_queue_.pop_back();
                sinfo.set_solved();
                uint8_t closed = LocalStateInfo::CLOSED;
                if (dead_end_listener_ != nullptr) {
                    auto& binfo = state_status_(stateid, sinfo);
                    if (!binfo.is_goal_state()
                        && !binfo.is_recognized_dead_end()) {
                        binfo.set_recognized_dead_end();
                        dead_end_listener_->operator()(state);
                    }
                    if (!binfo.is_goal_state()) {
                        closed = LocalStateInfo::CLOSED_DEAD;
                    }
                }
                return updated ? LocalStateInfo::UNSOLVED : closed;
            } else if (
                (!ExpandTipStates && is_tip_state)
                || (CutoffInconsistent && einfo.updated)) {
                transition_.clear();
                expansion_queue_.pop_back();
                return LocalStateInfo::UNSOLVED;
            }
            einfo.set_successors(transition_, this->state_id_map_);
            transition_.clear();
        } else {
            const auto& i = state_status_(stateid, sinfo);
            if (dead_end_listener_ == nullptr || !i.is_dead_end()) {
                transition_ =
                    this->transition_gen_->operator()(state, i.policy);
                einfo.set_successors(transition_, this->state_id_map_);
                transition_.clear();
            } else {
                assert(i.is_recognized_dead_end());
                sinfo.set_solved();
            }
        }
        return LocalStateInfo::ONSTACK;
    }

    template<bool UntilConvergence>
    bool value_iteration(const int start)
    {
        bool updated_all = false;
        bool updated = true;
        Action op = NullAction<Action>()();
        do {
            updated = false;
            for (int index = visited_.size() - 1; index >= start; --index) {
                statistics_.backtracking_updates++;
                DMSG(StateID id =
                         this->state_id_map_->operator[](visited_[index]);
                     std::cout << "updating " << id << " "
                               << (state_status_(id).get_value()) << " ... ";)
                const bool valupd =
                    this->async_update(visited_[index], &op, nullptr);
                DMSG(std::cout << (state_status_(id).get_value()) << " ["
                               << valupd << "|" << (op != policy_[index])
                               << std::endl;)
                updated = valupd || updated
                    || (DualBounds::value && this->interval_comparison_
                        && !this->state_status_(visited_[index])
                                .bounds_equal());
                updated_all = updated_all || (op != policy_[index]);
                if (!UntilConvergence && (updated || updated_all)) {
                    return true;
                }
            }
            updated_all = updated_all || updated;
        } while (updated);
        return updated_all;
    }

    const bool ForwardUpdates;
    const BacktrackingUpdateType BackwardUpdates;
    const bool CutoffInconsistent;
    const bool GreedyExploration;
    const bool PerformValueIteration;
    const bool ExpandTipStates;

    storage::PerStateStorage<PersistentStateInfo>* state_flags_;
    typename HeuristicSearchBase::template StateStatusAccessor<
        PersistentStateInfo>
        state_status_;
    StateListener<State, Action>* dead_end_listener_;
    const DeadEndIdentificationLevel dead_end_ident_level_;
    ExpansionCondition expansion_condition_;

    storage::StateHashMap<LocalStateInfo> state_infos_;
    std::vector<State> visited_;
    std::vector<Action> policy_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;
    Distribution<State> transition_;

    Statistics statistics_;
};
} // namespace internal

template<typename State, typename Action, typename B2>
using DepthOrientedHeuristicSearch = internal::DepthOrientedHeuristicSearch<
    heuristic_search_base::HeuristicSearchBase<
        State,
        Action,
        B2,
        std::true_type,
        internal::PerStateInformation>>;

template<typename State, typename Action, typename B2>
using DepthOrientedHeuristicSearchQ = internal::DepthOrientedHeuristicSearch<
    heuristic_search_base::
        HeuristicSearchBase<State, Action, B2, std::true_type>,
    internal::StandalonePerStateInformation>;

} // namespace depth_oriented_search
} // namespace algorithms
} // namespace probabilistic

#undef DMSG
