#pragma once

#include "quotient_heuristic_search_base.h"
#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"

#include <iostream>

namespace probabilistic {
namespace algorithms {
namespace gssp_lrtdp {

namespace internal {

struct Statistics : public IPrintable {
    unsigned long long trials;
    unsigned long long trial_bellman_backups;
    unsigned long long check_and_solve_bellman_backups;
    unsigned long long traps;

    Statistics()
        : trials(0)
        , trial_bellman_backups(0)
        , check_and_solve_bellman_backups(0)
        , traps(0)
    {
    }

    virtual void print(std::ostream& out) const override
    {
        out << "**** GSSP-LRTDP ****" << std::endl;
        out << "Trials: " << trials << std::endl;
        out << "Bellman backups (trials): " << trial_bellman_backups
            << std::endl;
        out << "Bellman backups (check&solved): "
            << check_and_solve_bellman_backups << std::endl;
        out << "Trap removals: " << traps << std::endl;
    }

    void register_report(ProgressReport* report) const
    {
        report->register_print([this](std::ostream& out) {
            out << "traps=" << traps << ", trials=" << trials;
        });
    }
};

template<typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr const uint8_t MARKED_OPEN = (1 << StateInfo::BITS);
    static constexpr const uint8_t MARKED_CLOSED = (2 << StateInfo::BITS);
    static constexpr const uint8_t SOLVED = (3 << StateInfo::BITS);
    static constexpr const uint8_t MARKED_TRIAL = (4 << StateInfo::BITS);
    static constexpr const uint8_t BITS = StateInfo::BITS + 3;
    static constexpr const uint8_t MASK = (3 << StateInfo::BITS);

    bool is_marked_open() const { return (this->info & MASK) == MARKED_OPEN; }
    bool is_marked_closed() const
    {
        return (this->info & MASK) == MARKED_CLOSED;
    }
    bool is_solved() const { return (this->info & MASK) == SOLVED; }
    void mark_open()
    {
        assert(!is_solved() && !is_marked_closed());
        this->info = (this->info & ~MASK) | MARKED_OPEN;
    }
    void mark_closed()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK) | MARKED_CLOSED;
    }
    void unmark()
    {
        assert(!is_solved());
        this->info = this->info & ~MASK;
    }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
    bool is_on_trial() const { return (this->info & MARKED_TRIAL); }
    void set_on_trial() { this->info = this->info | MARKED_TRIAL; }
    void clear_trial_flag() { this->info = (this->info & ~MARKED_TRIAL); }
};

} // namespace internal

template<typename StateT, typename ActionT, typename B2>
class LRTDP : public heuristic_search_base::QHeuristicSearchBase<
                  StateT,
                  ActionT,
                  B2,
                  std::true_type,
                  internal::PerStateInformation> {
public:
    using State = StateT;
    using Action = ActionT;
    using QuotientSystem = quotient_system::QuotientSystem<State, Action>;
    using QuotientSystemFunctionRegistry =
        quotient_system::QuotientSystemFunctionRegistry<State, Action>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;
    using HeuristicSearchBase = heuristic_search_base::QHeuristicSearchBase<
        State,
        Action,
        B2,
        std::true_type,
        internal::PerStateInformation>;
    using StateInfo = typename HeuristicSearchBase::StateInfo;
    using DualBounds = typename HeuristicSearchBase::DualBounds;

    template<typename... Args>
    LRTDP(
        bool stop_consistent,
        TransitionSampler<State, Action>* succ_sampler,
        StateListener<State, Action>* dead_end_listener,
        StateEvaluationFunction<State>* dead_end_reeval,
        DeadEndIdentificationLevel level,
        Args... args)
        : HeuristicSearchBase(args...)
        , stop_at_consistent_(stop_consistent)
        , state_infos_(this->template get_state_status_access<StateInfo>())
        , sample_(this->fn_registry_->create(succ_sampler))
        , level_(level)
        , listener_(this->fn_registry_->create(dead_end_listener))
        , dead_end_reeval_(this->fn_registry_->create(dead_end_reeval))
        , check_solved_(state_infos_)
        , selected_transition_()
        , current_trial_()
        , update_queue_()
        , expanded_()
        , unmark_queue_()
        , policy_state_indices_(-1)
        , expansion_stack_()
        , statistics_()
    {
        sample_->connect(this->interface());
    }

protected:
    virtual AnalysisResult qsolve(const QState& qs) override
    {
        this->initialize_report(qs);
        const StateID state_id = this->state_id_map_->operator[](qs);
        const auto& info = state_infos_(qs);
        while (!info.is_solved()) {
            trial(qs);
            statistics_.trials++;
            this->report(state_id);
        }
        return this->create_result(qs, new internal::Statistics(statistics_));
    }

private:
    using StateInfoStore =
        typename HeuristicSearchBase::template StateStatusAccessor<StateInfo>;

    struct CheckSolved {
        explicit CheckSolved(StateInfoStore& state_infos)
            : state_infos_(state_infos)
        {
        }
        bool operator()(const QState& state)
        {
            return state_infos_(state).is_solved();
        }
        StateInfoStore state_infos_;
    };

    struct CheckAndSolveStackInfo {
        explicit CheckAndSolveStackInfo(int index)
            : root(true)
            , solved(true)
            , dead(true)
            , leaf(true)
            , lowlink(index)
        {
        }
        void update(const CheckAndSolveStackInfo& child)
        {
            solved = solved && child.solved;
            leaf = leaf && child.leaf;
            dead = dead && child.dead;
            if (child.lowlink < lowlink) {
                lowlink = child.lowlink;
                root = false;
            }
        }
        void update_lowlink(int idx)
        {
            if (idx < lowlink) {
                lowlink = idx;
                root = false;
            }
        }

        bool root;
        bool solved;
        bool dead;
        bool leaf;
        int lowlink;
    };

    struct UpdateElement {
        explicit UpdateElement(const StateID& id, const QState& qs)
            : expanded(false)
            , stateid(id)
            , state(qs)
        {
        }
        bool expanded;
        StateID stateid;
        QState state;
    };

    void trial(const QState& init)
    {
        assert(current_trial_.empty());
        assert(selected_transition_.empty());
        current_trial_.push_back(this->state_id_map_->operator[](init));
        QState state = init;
        QAction action = NullAction<QAction>()();
        while (true) {
            StateID stateid = current_trial_.back();
            auto& info = state_infos_(stateid);
            if (info.is_solved()) {
                current_trial_.pop_back();
                break;
            }
            statistics_.trial_bellman_backups++;
            const bool changed =
                this->async_update(state, &action, &selected_transition_);
            if (selected_transition_.empty()) {
                info.set_solved();
                if (listener_ != nullptr && !info.is_goal_state()
                    && !info.is_recognized_dead_end()) {
                    info.set_recognized_dead_end();
                    listener_->operator()(state);
                }
                current_trial_.pop_back();
                break;
            }
            if (stop_at_consistent_) {
                if (!changed) {
                    selected_transition_.clear();
                    break;
                }
            } else {
                if (info.is_on_trial()) {
                    selected_transition_.clear();
                    current_trial_.pop_back();
                    break;
                }
                info.set_on_trial();
            }
            state = sample_->operator()(state, action, selected_transition_);
            current_trial_.push_back(this->state_id_map_->operator[](state));
            selected_transition_.clear();
        }
        while (!current_trial_.empty()) {
            if (!check_and_solve()) {
                break;
            }
            if (!stop_at_consistent_) {
                auto& info = state_infos_(current_trial_.back());
                assert(info.is_on_trial());
                info.clear_trial_flag();
            }
            current_trial_.pop_back();
        }
        if (!stop_at_consistent_) {
            while (!current_trial_.empty()) {
                auto& info = state_infos_(current_trial_.back());
                assert(info.is_on_trial());
                info.clear_trial_flag();
                current_trial_.pop_back();
            }
        }
        current_trial_.clear();
    }

    bool check_and_solve()
    {
        assert(!current_trial_.empty());
        assert(selected_transition_.empty());
        assert(update_queue_.empty());
        assert(expanded_.empty());
        assert(unmark_queue_.empty());
        assert(expansion_stack_.empty());
        assert(policy_state_indices_.empty());

        int index = 0;
        expansion_stack_.emplace_back(-1);
        update_queue_.emplace_back(
            current_trial_.back(),
            this->state_id_map_->operator[](current_trial_.back()));
        bool test_for_backjump = false;
        bool consistent = true;
        while (!update_queue_.empty()) {
            auto& elem = update_queue_.back();
            if (elem.expanded) {
                CheckAndSolveStackInfo stack_info(
                    std::move(expansion_stack_.back()));
                expansion_stack_.pop_back();
                CheckAndSolveStackInfo& parent_info = expansion_stack_.back();
                test_for_backjump = false;
                if (stack_info.root) {
                    const bool singleton = (expanded_.front() == elem.stateid);
                    auto end = expanded_.begin();
                    if (singleton) {
                        auto& state_info = state_infos_(elem.stateid);
                        if (consistent && stack_info.solved) {
                            assert(!stack_info.leaf);
                            assert(!state_info.is_dead_end());
                            if (listener_ == nullptr || !stack_info.dead) {
                                state_info.set_solved();
                                parent_info.dead = false;
                            } else if (this->safe_async_update(
                                           elem.state,
                                           *listener_,
                                           check_solved_)) {
                                unmark_queue_.push_back(elem.stateid);
                                state_info.mark_closed();
                                parent_info.solved = false;
                            } else {
                                assert(state_info.is_dead_end());
                                state_info.set_solved();
                                test_for_backjump = true;
                            }
                        } else {
                            state_info.mark_closed();
                            unmark_queue_.push_back(elem.stateid);
                            statistics_.check_and_solve_bellman_backups++;
                            parent_info.solved = false;
                            this->async_update(elem.state);
                        }
                        ++end;
                    } else if (consistent && stack_info.solved) {
                        if (stack_info.leaf) {
                            parent_info.solved = false;
                            statistics_.traps++;
                            do {
                                auto& state_info = state_infos_(*end);
                                assert(state_info.is_marked_open());
                                state_info.mark_closed();
                                unmark_queue_.push_back(*end);
                                if (*end == elem.stateid) {
                                    ++end;
                                    break;
                                }
                                ++end;
                            } while (true);
                            this->quotient_->build_quotient(
                                expanded_.begin(), end, elem.stateid);
                            unmark_queue_.pop_back();
                            state_infos_(elem.stateid).unmark();
                            elem.expanded = false;
                            expanded_.erase(expanded_.begin(), end);
                            continue;
                        } else {
                            if (listener_ == nullptr || !stack_info.dead) {
                                do {
                                    auto& state_info = state_infos_(*end);
                                    state_info.set_solved();
                                    if (*end == elem.stateid) {
                                        ++end;
                                        break;
                                    }
                                    ++end;
                                } while (true);
                                parent_info.dead = false;
                            } else if (this->safe_async_update(
                                           elem.state,
                                           *listener_,
                                           check_solved_)) {
                                // TODO mark solve here?
                                do {
                                    auto& state_info = state_infos_(*end);
                                    state_info.mark_closed();
                                    unmark_queue_.push_back(*end);
                                    if (*end == elem.stateid) {
                                        ++end;
                                        break;
                                    }
                                    ++end;
                                } while (true);
                                parent_info.solved = false;
                            } else {
                                do {
                                    auto& state_info = state_infos_(*end);
                                    state_info.set_solved();
                                    if (*end == elem.stateid) {
                                        ++end;
                                        break;
                                    }
                                    ++end;
                                } while (true);
                                test_for_backjump = true;
                            }
                        }
                    } else {
                        do {
                            auto& state_info = state_infos_(*end);
                            state_info.mark_closed();
                            unmark_queue_.push_back(*end);
                            statistics_.check_and_solve_bellman_backups++;
                            this->async_update(
                                this->state_id_map_->operator[](*end));
                            if (*end == elem.stateid) {
                                ++end;
                                break;
                            }
                            ++end;
                        } while (true);
                        parent_info.solved = false;
                    }
                    expanded_.erase(expanded_.begin(), end);
                    parent_info.leaf = false;
                } else {
                    parent_info.update(stack_info);
                }
                update_queue_.pop_back();
                if (dead_end_reeval_ != nullptr && test_for_backjump) {
                    auto uq_it = update_queue_.rbegin();
                    while (uq_it != update_queue_.rend()) {
                        if (uq_it->expanded) {
                            if (dead_end_reeval_->operator()(uq_it->state)) {
                                while (!expansion_stack_.back().root) {
                                    expansion_stack_.pop_back();
                                    ++uq_it;
                                    while (!uq_it->expanded)
                                        ++uq_it;
                                }
                                expansion_stack_.pop_back();
                                auto& parent_info = expansion_stack_.back();
                                parent_info.leaf = false;
                                parent_info.solved = false;
                                auto end = expanded_.begin();
                                do {
                                    auto& info = state_infos_(*end);
                                    this->set_dead_end(info);
                                    // info.set_recognized_dead_end(); // TODO ?
                                    info.mark_closed();
                                    unmark_queue_.push_back(*end);
                                } while (*(end++) != uq_it->stateid);
                                expanded_.erase(expanded_.begin(), end);
                                update_queue_.erase(
                                    --(uq_it.base()), update_queue_.end());
                                uq_it = update_queue_.rbegin();
                            } else {
                                break;
                            }
                        } else {
                            ++uq_it;
                        }
                    }
                }
            } else {
                auto& state_info = state_infos_(elem.stateid);
                CheckAndSolveStackInfo& parent_info = expansion_stack_.back();
                if (state_info.is_solved()) {
                    parent_info.leaf = false;
                    parent_info.dead &= state_info.is_dead_end();
                    update_queue_.pop_back();
                } else if (state_info.is_marked_open()) {
                    parent_info.update_lowlink(
                        policy_state_indices_[elem.stateid]);
                    update_queue_.pop_back();
                } else if (state_info.is_marked_closed()) {
                    parent_info.solved = false;
                    update_queue_.pop_back();
                } else {
                    statistics_.check_and_solve_bellman_backups++;
                    const bool val_changed = this->async_update(
                        elem.state, nullptr, &selected_transition_);
                    if (val_changed) {
                        // std::cout << "  value changed! (" <<
                        // selected_transition_.size()
                        //           << ")" << std::endl;
                        state_info.mark_closed();
                        unmark_queue_.push_back(elem.stateid);
                        parent_info.solved = false;
                        consistent = false;
                        assert(!state_info.is_goal_state());
                        if (listener_ != nullptr
                            && !state_info.is_recognized_dead_end()
                            && selected_transition_.empty()) {
                            listener_->operator()(elem.state);
                            state_info.set_recognized_dead_end();
                        }
                        update_queue_.pop_back();
                    } else if (selected_transition_.empty()) {
                        state_info.set_solved();
                        parent_info.leaf = false;
                        if (state_info.is_goal_state()) {
                            parent_info.dead = false;
                        } else if (
                            listener_ != nullptr
                            && !state_info.is_recognized_dead_end()) {
                            listener_->operator()(elem.state);
                            state_info.set_recognized_dead_end();
                        }
                        update_queue_.pop_back();
                    } else if (consistent) {
                        elem.expanded = true;
                        if (DualBounds::value && this->interval_comparison_
                            && !state_info.bounds_equal()) {
                            parent_info.solved = false;
                        }
                        state_info.mark_open();
                        policy_state_indices_[elem.stateid] = index;
                        expansion_stack_.emplace_back(index++);
                        expanded_.push_front(elem.stateid);
                        // std::cout << "expanding " << elem.second << " ->  [";
                        for (auto it = selected_transition_.begin();
                             it != selected_transition_.end();
                             ++it) {
                            // std::cout << " " << (it->first);
                            update_queue_.emplace_back(
                                this->state_id_map_->operator[](it->first),
                                it->first);
                        }
                        // std::cout << " ]" << std::endl;
                    } else {
                        update_queue_.pop_back();
                    }
                    selected_transition_.clear();
                }
            }
        }

        assert(expanded_.empty());

        while (!unmark_queue_.empty()) {
            state_infos_(unmark_queue_.back()).unmark();
            unmark_queue_.pop_back();
        }

#if 0
#if !defined(NDEBUG)
        for (auto it = policy_state_indices_.begin();
             it != policy_state_indices_.end();
             ++it) {
            const auto& info = state_infos_->operator[](it->first);
            assert(!info.is_marked_open() && !info.is_marked_closed());
        }
#endif
#endif

        policy_state_indices_.clear();

        assert(expansion_stack_.size() == 1);
        bool result = expansion_stack_.back().solved;
        expansion_stack_.pop_back();

        if (test_for_backjump && current_trial_.size() > 1
            && dead_end_reeval_ != nullptr) {
            StateID lsid = current_trial_.back();
            current_trial_.pop_back();
            while (!current_trial_.empty()) {
                QState qs = this->state_id_map_->operator[](lsid);
                if (dead_end_reeval_->operator()(qs)) {
                    lsid = current_trial_.back();
                    current_trial_.pop_back();
                    this->set_dead_end(lsid);
                    result = false;
                } else {
                    break;
                }
            }
            current_trial_.push_back(lsid);
        }

        return result;
    }

    const bool stop_at_consistent_;

    StateInfoStore state_infos_;
    TransitionSampler<QState, QAction>* sample_;
    const DeadEndIdentificationLevel level_;
    StateListener<QState, QAction>* listener_;
    StateEvaluationFunction<QState>* dead_end_reeval_;

    CheckSolved check_solved_;

    Distribution<QState> selected_transition_;

    std::deque<StateID> current_trial_;
    std::deque<UpdateElement> update_queue_;
    std::deque<StateID> expanded_;
    std::deque<StateID> unmark_queue_;
    storage::StateHashMap<int> policy_state_indices_;
    std::deque<CheckAndSolveStackInfo> expansion_stack_;

    internal::Statistics statistics_;
};

} // namespace gssp_lrtdp
} // namespace algorithms
} // namespace probabilistic
