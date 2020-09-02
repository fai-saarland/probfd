#pragma once

#include "quotient_heuristic_search_base.h"

#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>

#define DMSG(x)

namespace probabilistic {
namespace algorithms {
namespace gssp_dohs {

enum class BacktrackingUpdateType {
    Disabled,
    Single,
    UntilConvergence,
};

namespace internal {

struct Statistics : public IPrintable {
    virtual void print(std::ostream& out) const override
    {
        out << "**** GSSP Depth-Oriented Heuristic Search ****" << std::endl;
        out << "Iterations: " << iterations << std::endl;
        out << "Total number of SCCs: " << sccs << " (" << scc1
            << " singletons)" << std::endl;
        out << "Traps: " << traps << std::endl;
        out << "Value iterations (backtracking): " << bw_updates << std::endl;
        out << "Bellman backups (forward): " << fw_updates << std::endl;
    }

    void register_report(ProgressReport* report) const
    {
        report->register_print([this](std::ostream& out) {
            out << "iteration=" << iterations << ", traps=" << traps;
        });
    }

    unsigned long long iterations = 0;
    unsigned long long traps = 0;
    unsigned long long scc1 = 0;
    unsigned long long sccs = 0;
    unsigned long long fw_updates = 0;
    unsigned long long bw_updates = 0;
};

template<typename BaseInfo>
struct PerStateInformation : public BaseInfo {
    static constexpr const uint8_t INITIALIZED = (1 << BaseInfo::BITS);
    static constexpr const uint8_t SOLVED = (2 << BaseInfo::BITS);
    static constexpr const uint8_t BITS = BaseInfo::BITS + 2;
    static constexpr const uint8_t MASK = (3 << BaseInfo::BITS);

    bool is_tip() const { return (this->info & MASK) != 0; }
    bool is_solved() const { return this->info & SOLVED; }
    void set_expanded() { this->info = (this->info & ~MASK) | INITIALIZED; }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
    void reset() { this->info = (this->info & ~MASK); }
};

template<typename SuccessorList>
struct StateExpansionInfoT {

    StateExpansionInfoT(SuccessorList&& successors)
        : successors(std::move(successors))
        , it(this->successors.begin())
        , end(this->successors.end())
    {
    }

    bool leaf = true;
    bool dead = true;
    bool has_unsolved_successor = false;

    unsigned stck = (unsigned)-1;
    unsigned lstck = (unsigned)-1;

    SuccessorList successors;
    typename SuccessorList::const_iterator it;
    typename SuccessorList::const_iterator end;
};

template<typename SuccessorList>
using ExpansionQueueT = std::vector<StateExpansionInfoT<SuccessorList>>;

using ExpansionStack = std::deque<StateID>;

struct StackIdx {
    static constexpr const unsigned UNDEF = (((unsigned)-1) >> 1);
    static constexpr const unsigned TRAPPED = UNDEF - 1;
    StackIdx()
        : explored(0)
        , idx(UNDEF)
    {
    }
    inline bool onstack() const { return idx < UNDEF; }
    inline bool trapped() const { return idx == TRAPPED; }
    inline unsigned stack_idx() const
    {
        assert(idx < TRAPPED);
        return idx;
    }
    unsigned explored : 1;
    unsigned idx : 31;
};

using StackIdxMap = storage::StateHashMap<StackIdx>;

struct MultiSuccessorList {
#if 0

    explicit MultiSuccessorList()
        : groups(0)
        , size(nullptr)
        , successors(nullptr)
    {
    }

    MultiSuccessorList(MultiSuccessorList&& other)
        : groups(other.groups)
        , size(other.size)
        , successors(other.successors)
    {
    }

    MultiSuccessorList(const MultiSuccessorList&
#if !defined(NDEBUG)
                           other
#endif
                       )
        : MultiSuccessorList()
    {
        assert(other.size == nullptr);
    }

    explicit MultiSuccessorList(
        unsigned total,
        const std::vector<std::vector<StateID>>& ids)
        : groups(ids.size())
        , size(new unsigned[ids.size() + 1])
        , successors(new StateID[total])
    {
        assert(!ids.empty());
        StateID* out = successors;
        unsigned* n = size;
        const auto* outer = &ids.back();
        for (int i = ids.size(); i > 0; --i, --outer, ++n) {
            const auto* it = &outer->back();
            *n = outer->size();
            for (int j = outer->size(); j > 0; --j, --it, ++out) {
                *out = *it;
            }
        }
        *n = total;
    }

    ~MultiSuccessorList()
    {
        if (size != nullptr) {
            delete[](size);
        }
        if (successors != nullptr) {
            delete[](successors);
        }
    }

    const_iterator begin() const
    {
        return const_iterator(
            size + groups - 1, successors_ + *(size + groups));
    }

    const_iterator end() const
    {
        return const_iterator(size - 1, successors_ - 1);
    }

    void next_group(const_iterator& iterator) const
    {
        iterator.successors_ = iterator.successors_ - *iterator.size_;
        iterator.left_ = 0;
    }

    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = StateID;
        using difference_type = int;
        using pointer = const StateID*;
        using reference = const StateID&;

        explicit const_iterator(const unsigned* size, const StateID* ids)
            : left_(0)
            , size_(size + 1)
            , ids_(ids)
        {
        }

        reference operator*() const { return *ids_; }
        const_iterator& operator++()
        {
            if (left_ == 0) {
                --size_;
                left_ = *size_;
            }
            --left_;
            --successors_;
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator copy(*this);
            ++(*this);
            return copy;
        }
        bool operator==(const const_iterator& other) const
        {
            return successors_ == other.successors_;
        }
        bool operator!=(const const_iterator& other) const
        {
            return successors_ != other.successors_;
        }

    private:
        friend class MultiSuccessorList;

        unsigned left_;
        const unsigned* size_;
        const StateID* successors_;
    };

    unsigned groups;
    unsigned* size;
    StateID* successors;
#endif
};

template<
    typename StateT,
    typename ActionT,
    typename DualBoundsT,
    typename SuccessorList>
class DepthOrientedHeuristicSearch
    : public heuristic_search_base::QHeuristicSearchBase<
          StateT,
          ActionT,
          DualBoundsT,
          std::true_type,
          PerStateInformation> {
public:
    using QuotientSystem = quotient_system::QuotientSystem<StateT, ActionT>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;
    using HeuristicSearchBase = heuristic_search_base::QHeuristicSearchBase<
        StateT,
        ActionT,
        DualBoundsT,
        std::true_type,
        PerStateInformation>;
    using StateExpansionInfo = StateExpansionInfoT<SuccessorList>;
    using ExpansionQueue = ExpansionQueueT<SuccessorList>;
    using HasGroups = std::is_same<SuccessorList, MultiSuccessorList>;
    using StateInfo = typename HeuristicSearchBase::StateInfo;
    using StateInfoStore =
        typename HeuristicSearchBase::template StateStatusAccessor<StateInfo>;

    template<typename... Args>
    DepthOrientedHeuristicSearch(
        bool forward_updates,
        bool expand_tip_states,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool value_iteration,
        bool mark_solved,
        bool force_backtrack_updates,
        BacktrackingUpdateType backtrack_update_type,
        OpenList<StateT, ActionT>* open_list,
        StateListener<StateT, ActionT>* listener,
        DeadEndIdentificationLevel level,
        Args... args)
        : HeuristicSearchBase(args...)
        , forward_updates_(forward_updates)
        , expand_tip_states_(expand_tip_states)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , stop_exploration_inconsistent_(stop_exploration_inconsistent)
        , value_iteration_(value_iteration)
        , mark_solved_(mark_solved)
        , force_backtrack_updates_(force_backtrack_updates)
        , backtrack_update_type_(backtrack_update_type)
        , open_list_(this->fn_registry_->create(open_list))
        , listener_(this->fn_registry_->create(listener))
        , state_infos_(this->template get_state_status_access<StateInfo>())
        , expansion_condition_(
              level,
              &stack_idx_,
              this->state_id_map_,
              state_infos_)
    {
    }

protected:
    virtual AnalysisResult qsolve(const QState& qstate) override
    {
        this->initialize_report(qstate);
        if (value_iteration_) {
            dohs_vi(qstate);
        } else {
            dohs_labels(qstate);
        }
        return this->create_result(qstate, new Statistics(stats_));
    }

    virtual void setup_custom_reports(const QState&)
    {
        stats_.register_report(this->report_);
    }

private:
    StateID lookup(const StateID& id, StackIdx*& idx)
    {
        idx = &stack_idx_[id];
        if (idx->trapped()) {
            const StateID repr = this->quotient_->get_representative_id(id);
            idx = &stack_idx_[repr];
            return repr;
        }
        return id;
    }

    void dohs_vi(const QState& state)
    {
        const StateID state_id = this->state_id_map_->operator[](state);
        bool unsolved = true;
        do {
            if (push_initial_state(state)) {
                exploration();
                unsolved = value_iteration(
                    false, visited_states_.begin(), visited_states_.end());
            }
            visited_states_.clear();
            queue_.clear();
            ++stats_.iterations;
            this->report(state_id);
        } while (unsolved);
    }

    void dohs_labels(const QState& state)
    {
        const StateID state_id = this->state_id_map_->operator[](state);
        bool unsolved = false;
        do {
            if (push_initial_state(state)) {
                exploration();
            }
            unsolved = queue_.back().has_unsolved_successor;
            queue_.pop_back();
            ++stats_.iterations;
            this->report(state_id);
        } while (unsolved);
    }

    bool push_initial_state(const QState& state)
    {
        assert(queue_.empty());
        assert(stack_.empty());
        const StateID state_id = this->state_id_map_->operator[](state);
        StateInfo& info = state_infos_(state_id);
        assert(!info.is_solved());
        queue_.emplace_back(SuccessorList());
        if (expand(&queue_.back(), state_id, info)) {
            StackIdx& idx = stack_idx_[state_id];
            StateExpansionInfo& e = queue_.back();
            idx.idx = e.stck = e.lstck = 0;
            idx.explored = 1;
            stack_.push_back(state_id);
            return true;
        }
        return false;
    }

    void backtrack_from_singleton(
        StateExpansionInfo* expanding,
        StateExpansionInfo& info)
    {
        const StateID state_id = stack_.back();
        const QState state = this->state_id_map_->operator[](state_id);
        if (!info.has_unsolved_successor && info.dead && listener_ != nullptr) {
            std::pair<bool, bool> updated;
            this->safe_async_update(
                state, *listener_, expansion_condition_, updated);
            info.dead = !updated.first;
            info.has_unsolved_successor = updated.first;
        } else {
            info.dead = 0;
        }
        if (!info.dead) {
            if (value_iteration_) {
                visited_states_.push_back(state);
            }
            if (backtrack_update_type_ != BacktrackingUpdateType::Disabled
                && (force_backtrack_updates_ || info.has_unsolved_successor)) {
                ++stats_.bw_updates;
                bool policy_changed = false;
                const bool updated = this->async_update(
                    state, nullptr, nullptr, &policy_changed);
                info.has_unsolved_successor |= (updated || policy_changed);
            }
            expanding->dead = false;
        }
        expanding->has_unsolved_successor |= info.has_unsolved_successor;
        expanding->leaf = false;
        stack_idx_[state_id].idx = StackIdx::UNDEF;
        if (!info.has_unsolved_successor && mark_solved_) {
            state_infos_(state_id).set_solved();
        }
        stack_.pop_back();
        ++stats_.scc1;
    }

    void backtrack_from_trap(
        StateExpansionInfo*& expanding,
        StateExpansionInfo& info)
    {
        std::deque<StateID>::const_iterator scc_begin =
            stack_.begin() + info.stck;
        const StateID repr_id = *scc_begin;
        for (auto it = scc_begin; it != stack_.end(); ++it) {
            StackIdx& idx = stack_idx_[*it];
            idx.idx = StackIdx::TRAPPED;
        }
        this->quotient_->build_quotient(scc_begin, stack_.cend(), repr_id);
        stack_.erase(scc_begin, stack_.end());
        StateInfo& state_info = state_infos_(repr_id);
        StackIdx& idx = stack_idx_[repr_id];
        state_info.reset();
        if (expand(expanding, repr_id, state_info)) {
            expanding = &queue_.back();
            idx.idx = expanding->stck = expanding->lstck = stack_.size();
            stack_.push_back(repr_id);
        } else {
            idx.idx = StackIdx::UNDEF;
        }
        ++stats_.traps;
    }

    void backtrack_from_scc(
        StateExpansionInfo* expanding,
        StateExpansionInfo& info,
        const unsigned scc_size)
    {
        const std::deque<StateID>::const_iterator scc_begin =
            stack_.begin() + info.stck;
        const StateID repr_id = *scc_begin;
        if (!info.has_unsolved_successor && info.dead && listener_ != nullptr) {
            const QState state = this->state_id_map_->operator[](repr_id);
            std::pair<bool, bool> updated;
            this->safe_async_update(
                state, *listener_, expansion_condition_, updated);
            info.dead = !updated.first;
            info.has_unsolved_successor = updated.first;
        } else {
            info.dead = false;
        }
        if (!info.dead) {
            const bool run_vi = backtrack_update_type_
                    == BacktrackingUpdateType::UntilConvergence
                && (info.has_unsolved_successor || force_backtrack_updates_);
            if (value_iteration_ || run_vi) {
                add_to_visited(scc_size);
                if (run_vi) {
                    info.has_unsolved_successor =
                        value_iteration(
                            true,
                            visited_states_.begin()
                                + (visited_states_.size() - scc_size),
                            visited_states_.end())
                        || info.has_unsolved_successor;
                    if (!value_iteration_) {
                        visited_states_.clear();
                    }
                }
            }
            expanding->dead = false;
        }
        expanding->has_unsolved_successor |= info.has_unsolved_successor;
        expanding->leaf = false;
        if (!info.has_unsolved_successor && mark_solved_) {
            for (auto scc_it = scc_begin; scc_it != stack_.end(); ++scc_it) {
                StateInfo& info = state_infos_(*scc_it);
                StackIdx& idx = stack_idx_[*scc_it];
                info.set_solved();
                idx.idx = StackIdx::UNDEF;
            }
        } else {
            for (auto scc_it = scc_begin; scc_it != stack_.end(); ++scc_it) {
                StackIdx& idx = stack_idx_[*scc_it];
                idx.idx = StackIdx::UNDEF;
            }
        }
        stack_.erase(scc_begin, stack_.end());
    }

    void exploration()
    {
        assert(queue_.size() == 2);
        assert(stack_.size() == 1);
        StateExpansionInfo* expanding = &queue_.back();
        StackIdx* succ_idx = nullptr;
        do {
            DMSG(std::cout << queue_.size() << "." << expanding->stck << "..."
                           << std::flush;)
            while (expanding->it != expanding->end) {
                const StateID succ_id = lookup(*expanding->it, succ_idx);
                DMSG(std::cout << stack_[expanding->stck] << " -> " << succ_id
                               << std::endl;)
                ++expanding->it;
                StateInfo& succ_info = state_infos_(succ_id);
                if (succ_info.is_solved()) {
                    expanding->dead &= succ_info.is_dead_end();
                    expanding->leaf = false;
                    continue;
                }
                if (!succ_idx->explored) {
                    succ_idx->explored = 1;
                    if (expand(expanding, succ_id, succ_info)) {
                        expanding = &queue_.back();
                        expanding->stck = stack_.size();
                        expanding->lstck = stack_.size();
                        succ_idx->idx = stack_.size();
                        stack_.push_back(succ_id);
                    } else if (
                        stop_exploration_inconsistent_
                        && expanding->has_unsolved_successor) {
                        break;
                    }
                } else if (succ_idx->onstack()) {
                    expanding->lstck =
                        std::min(expanding->lstck, succ_idx->stack_idx());
                } else {
                    expanding->leaf = false;
                    expanding->dead &= succ_info.is_dead_end();
                    expanding->has_unsolved_successor |= !succ_info.is_solved();
                }
            }
            DMSG(std::cout << "Backtracking from " << stack_[expanding->stck]
                           << ": stck=" << expanding->stck << " , lstck="
                           << expanding->lstck << ", size=" << stack_.size()
                           << ", flags=" << expanding->has_unsolved_successor
                           << "|" << expanding->leaf << "|" << expanding->dead
                           << std::endl;)
            StateExpansionInfo info(std::move(*expanding));
            queue_.pop_back();
            expanding = &queue_.back();
            if (info.stck == info.lstck) {
                const unsigned scc_size = stack_.size() - info.stck;
                if (scc_size == 1) {
                    DMSG(std::cout << "  => singleton" << std::endl;)
                    backtrack_from_singleton(expanding, info);
                } else {
                    if (!info.has_unsolved_successor && info.leaf) {
                        DMSG(std::cout << "  => trap(" << scc_size << ")"
                                       << std::endl;)
                        backtrack_from_trap(expanding, info);
                    } else {
                        DMSG(std::cout << "  => scc(" << scc_size << ")"
                                       << std::endl;)
                        backtrack_from_scc(expanding, info, scc_size);
                    }
                }
                ++stats_.sccs;
            } else {
                if (backtrack_update_type_ == BacktrackingUpdateType::Single
                    && (info.has_unsolved_successor
                        || force_backtrack_updates_)) {
                    ++stats_.bw_updates;
                    const StateID state_id = stack_[info.stck];
                    const QState state =
                        this->state_id_map_->operator[](state_id);
                    bool policy_changed = false;
                    const bool updated = this->async_update(
                        state, nullptr, nullptr, &policy_changed);
                    expanding->has_unsolved_successor |=
                        (updated || policy_changed);
                }
                expanding->dead &= info.dead;
                expanding->leaf &= info.leaf;
                expanding->has_unsolved_successor |=
                    info.has_unsolved_successor;
                expanding->lstck = std::min(expanding->lstck, info.lstck);
            }
        } while (queue_.size() > 1);
        assert(stack_.empty());
        stack_idx_.clear();
    }

    inline bool
    expand(StateExpansionInfo* parent, const StateID sid, StateInfo& info)
    {
        return expand(HasGroups(), parent, sid, info);
    }

    bool expand(
        const std::false_type&,
        StateExpansionInfo* parent,
        const StateID sid,
        StateInfo& info)
    {
        assert(!info.is_solved());
        if (info.is_goal_state()) {
            info.set_solved();
            parent->leaf = false;
            parent->dead = false;
            return false;
        }
        const bool is_tip = info.is_tip();
        const QState state = this->state_id_map_->operator[](sid);
        QAction policy = NullAction<QAction>()();
        if (info.is_dead_end()) {
            assert(info.is_recognized_dead_end());
            info.set_solved();
            parent->leaf = false;
            return false;
        } else if (is_tip || forward_updates_) {
            info.set_expanded();
            ++stats_.fw_updates;
            const bool updated =
                this->async_update(state, &policy, &transition_);
            parent->has_unsolved_successor |= updated;
            if (transition_.empty()) {
                info.set_solved();
                parent->leaf = false;
                if (info.is_goal_state()) {
                    parent->dead = false;
                } else if (
                    listener_ != nullptr && !info.is_recognized_dead_end()) {
                    this->set_dead_end(info);
                    listener_->operator()(state);
                }
                return false;
            } else if (
                (!expand_tip_states_ && is_tip)
                || (cutoff_inconsistent_ && updated)) {
                assert(!info.is_dead_end());
                parent->leaf = false;
                parent->dead = false;
                parent->has_unsolved_successor = true;
                transition_.clear();
                return false;
            }
        } else {
            policy = info.policy;
            transition_ = this->transition_gen_->operator()(state, policy);
        }
        assert(!transition_.empty());
        for (auto it = transition_.begin(); it != transition_.end(); ++it) {
            const StateID succ = this->state_id_map_->operator[](it->first);
            if (succ != sid) {
                open_list_->push(succ, state, policy, it->second, it->first);
            }
        }
        transition_.clear();
        assert(!open_list_->empty());
        std::vector<StateID> succs;
        succs.reserve(open_list_->size());
        do {
            succs.push_back(open_list_->pop());
        } while (!open_list_->empty());
        queue_.emplace_back(SuccessorList(std::move(succs)));
        return true;
    }

    void add_to_visited(unsigned num)
    {
        for (auto it = stack_.rbegin(); num > 0; --num, ++it) {
            visited_states_.push_back(this->state_id_map_->operator[](*it));
        }
    }

    bool value_iteration(
        const bool until_convergence,
        const typename std::vector<QState>::const_iterator begin,
        const typename std::vector<QState>::const_iterator end)
    {
        bool changed = false;
        bool last_iteration_changed = false;
        do {
            last_iteration_changed = false;
            for (auto it = begin; it != end; ++it) {
                bool c = false;
                last_iteration_changed =
                    this->async_update(*it, nullptr, nullptr, &c)
                    || last_iteration_changed || c;
                ++stats_.bw_updates;
            }
            changed = changed || last_iteration_changed;
        } while (last_iteration_changed && until_convergence);
        return changed;
    }

    struct ExpansionCondition {

        explicit ExpansionCondition(
            const DeadEndIdentificationLevel level,
            StackIdxMap* stack,
            StateIDMap<QState>* state_id_map,
            StateInfoStore& state_status)
            : level_(level)
            , stack_(stack)
            , state_id_map_(state_id_map)
            , state_status_(state_status)
        {
        }

        bool operator()(const QState& state)
        {
            const StateID stateid = state_id_map_->operator[](state);
            const StateInfo& state_info = state_status_(stateid);
            assert(!state_info.is_dead_end());
            if (state_info.is_solved()) {
                return true;
            }
            switch (level_) {
            case (DeadEndIdentificationLevel::Policy): {
                return !stack_->contains(stateid);
            }
            case (DeadEndIdentificationLevel::Visited): {
                return !state_info.is_value_initialized();
            }
            default:
                break;
            }
            return false;
        }

        const DeadEndIdentificationLevel level_;

        StackIdxMap* stack_;
        StateIDMap<QState>* state_id_map_;
        StateInfoStore state_status_;
    };

    const bool forward_updates_;
    const bool expand_tip_states_;
    const bool cutoff_inconsistent_;
    const bool stop_exploration_inconsistent_;
    const bool value_iteration_;
    const bool mark_solved_;
    const bool force_backtrack_updates_;
    const BacktrackingUpdateType backtrack_update_type_;

    OpenList<QState, QAction>* open_list_;
    StateListener<QState, QAction>* listener_;
    StateInfoStore state_infos_;

    ExpansionQueue queue_;
    ExpansionStack stack_;
    StackIdxMap stack_idx_;

    ExpansionCondition expansion_condition_;

    std::vector<QState> visited_states_;

    Distribution<QState> transition_;

    Statistics stats_;
};

} // namespace internal

template<typename State, typename Action, typename DualBounds>
using DepthOrientedHeuristicSearch = internal::DepthOrientedHeuristicSearch<
    State,
    Action,
    DualBounds,
    std::vector<StateID>>;

} // namespace gssp_dohs

} // namespace algorithms
} // namespace probabilistic

#undef DMSG
