#pragma once

#include "heuristic_search_base.h"
#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>

#define CAS_DEBUG_PRINT(x)

namespace probabilistic {
namespace algorithms {
namespace lrtdp {

namespace internal {

struct Statistics : public IPrintable {
    unsigned long long trials;
    unsigned long long trial_bellman_backups;
    unsigned long long check_and_solve_bellman_backups;
    unsigned long long state_info_bytes;

    Statistics()
        : trials(0)
        , trial_bellman_backups(0)
        , check_and_solve_bellman_backups(0)
        , state_info_bytes(0)
    {
    }

    virtual void print(std::ostream& out) const override
    {
        out << "**** LRTDP ****" << std::endl;
        out << "Additional per state information: " << state_info_bytes
            << " bytes" << std::endl;
        out << "Trials: " << trials << std::endl;
        out << "Bellman backups (trials): " << trial_bellman_backups
            << std::endl;
        out << "Bellman backups (check&solved): "
            << check_and_solve_bellman_backups << std::endl;
    }

    void register_report(ProgressReport& report)
    {
        report.register_print(
            [this](std::ostream& out) { out << "trials=" << trials; });
    }
};

struct EmptyStateInfo {
    static constexpr const uint8_t BITS = 0;
    EmptyStateInfo()
        : info(0)
    {
    }
    uint8_t info;
};

template<typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr const uint8_t MARKED_OPEN = (1 << StateInfo::BITS);
    static constexpr const uint8_t MARKED_CLOSED = (2 << StateInfo::BITS);
    static constexpr const uint8_t SOLVED = (3 << StateInfo::BITS);
    static constexpr const uint8_t BITS = StateInfo::BITS + 2;
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
};

template<
    typename HeuristicSearchBase,
    typename StateInfo = typename HeuristicSearchBase::StateInfo>
class LRTDP : public HeuristicSearchBase {
public:
    using State = typename HeuristicSearchBase::State;
    using Action = typename HeuristicSearchBase::Action;
    using DualBounds = typename HeuristicSearchBase::DualBounds;

    template<typename... Args>
    LRTDP(
        bool stop_consistent,
        TransitionSampler<State, Action>* succ_sampler,
        StateListener<State, Action>* dead_end_listener,
        StateEvaluationFunction<State>* dead_end_identifier,
        DeadEndIdentificationLevel level,
        Args... args)
        : HeuristicSearchBase(args...)
        , StopConsistent(stop_consistent)
        , state_infos_(nullptr)
        , sample_(succ_sampler)
        , listener_(dead_end_listener)
        , dead_end_identifier_(dead_end_identifier)
        , dead_end_identification_level_(level)
        , expansion_condition_(this)
        , state_status_(this->template get_state_status_access<StateInfo>())
    {
        if (sample_) {
            sample_->connect(this->interface());
        }
        this->setup_state_info_store(
            typename std::is_same<
                StateInfo,
                typename HeuristicSearchBase::StateInfo>::type());
    }

    ~LRTDP()
    {
        this->cleanup_state_info_store(
            typename std::is_same<
                StateInfo,
                typename HeuristicSearchBase::StateInfo>::type());
    }

    virtual AnalysisResult solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID state_id = this->state_id_map_->operator[](state);
        do {
            StateInfo& info = this->state_infos_->operator[](state_id);
            if (info.is_solved()) {
                break;
            }
            this->trial(state);
            this->statistics_.trials++;
            this->report(state_id);
        } while (true);
        return this->create_result(state, new Statistics(statistics_));
    }

    virtual void reset_solver_state() override
    {
        delete (this->state_infos_);
        this->state_infos_ = new storage::PerStateStorage<StateInfo>();
    }

#if 0
    Statistics operator()(const State& initial_state)
    {
        auto silent = []() {};
        return this->operator()(init, silent);
    }

    template<typename ProgressReport>
    Statistics operator()(const State& initial_state, ProgressReport& report)
    {
        const auto& info = state_infos_->operator[](init);
        while (!info.is_solved()) {
            trial(init);
            statistics_.trials++;
            report();
        }
        return statistics_;
    }

    void set_per_state_information_store(StateInfoStore* store)
    {
        state_infos_ = store;
    }

    StateInfoStore* get_per_state_information_store() const
    {
        return state_infos_;
    }
#endif

protected:
    virtual void setup_custom_reports(const State&) override
    {
        this->statistics_.register_report(*this->report_);
    }

private:
    struct ExpandInDeadEndCheck {

        explicit ExpandInDeadEndCheck(LRTDP* lrtdp)
            : lrtdp_(lrtdp)
            , level_(lrtdp->dead_end_identification_level_)
        {
        }

        bool operator()(const State& state) const
        {
            const StateID state_id = lrtdp_->state_id_map_->operator[](state);
            auto& lrtdp_info = lrtdp_->state_infos_->operator[](state_id);
            // std::cout << "visit " << state_id << " ==> ";
            if (lrtdp_info.is_solved()) {
                // std::cout << "solved!!!" << std::endl;
                return true;
            }
            switch (level_) {
            case (DeadEndIdentificationLevel::Policy):
                // std::cout << lrtdp_info.is_marked_open() << std::endl;
                return !lrtdp_info.is_marked_open();
            case (DeadEndIdentificationLevel::Visited): {
                // std::cout << lrtdp_->state_status_(state_id, lrtdp_info)
                //             .is_value_initialized() << std::endl;
                return !lrtdp_->state_status_(state_id, lrtdp_info)
                            .is_value_initialized();
            }
            default:
                break;
            }
            // std::cout << "expand" << std::endl;
            return false;
        }

        LRTDP* lrtdp_;
        const DeadEndIdentificationLevel level_;
    };

    void trial(const State& initial_state)
    {
        assert(
            this->current_trial_.empty() && this->selected_transition_.empty());
        this->current_trial_.push_back(initial_state);
        Action action = NullAction<Action>()();
        // std::cout << "trial = [";
        while (true) {
            const State& state = this->current_trial_.back();
            const StateID state_id = this->state_id_map_->operator[](state);
            // std::cout << " " << state;
            auto& state_info = state_infos_->operator[](state_id);
            if (state_info.is_solved()) {
                this->current_trial_.pop_back();
                break;
            }
            this->statistics_.trial_bellman_backups++;
            const bool value_changed =
                this->async_update(state, &action, &this->selected_transition_);
            if (this->selected_transition_.empty()) {
                // terminal
                assert(this->state_status_(state_id, state_info).is_terminal());
                this->check_goal_or_mark_dead_end(state, state_id, state_info);
                state_info.set_solved();
                this->current_trial_.pop_back();
                break;
            }
            assert(!this->state_status_(state_id, state_info).is_terminal());
            if (!value_changed && StopConsistent) {
                this->selected_transition_.clear();
                break;
            }
            this->current_trial_.push_back(
                sample_->operator()(state, action, this->selected_transition_));
            this->selected_transition_.clear();
        }
        // std::cout << " ]" << std::endl;
        this->last_check_and_solve_was_dead_ = true;
        while (!this->current_trial_.empty()) {
            bool solved = this->check_and_solve();
            this->current_trial_.pop_back();
            if (this->dead_end_identifier_ != nullptr
                && this->last_check_and_solve_was_dead_) {
                while (!this->current_trial_.empty()) {
                    if (this->dead_end_identifier_->operator()(
                            this->current_trial_.back())) {
                        this->set_dead_end(
                            this->state_status_(current_trial_.back()));
                        this->current_trial_.pop_back();
                        solved = false;
                    } else {
                        break;
                    }
                }
            }
            if (!solved) {
                break;
            }
        }
        this->current_trial_.clear();
    }

    bool check_and_solve()
    {
        assert(!this->current_trial_.empty());
        assert(this->selected_transition_.empty());
        assert(this->policy_queue_.empty());
        assert(this->visited_.empty());

        CAS_DEBUG_PRINT(std::cout << ".... (C&S): "
                                  << this->current_trial_.back() << std::endl;)

        bool mark_solved = true;
        bool epsilon_consistent = true;
        bool all_dead = this->last_check_and_solve_was_dead_
            && (!StopConsistent
                || this->has_dead_end_value(this->current_trial_.back()));
        bool any_dead = false;
        this->last_check_and_solve_was_dead_ = false;

        this->policy_queue_.emplace_back(false, this->current_trial_.back());
        while (!this->policy_queue_.empty()) {
            auto& elem = this->policy_queue_.back();
            if (elem.first) {
                this->visited_.push_back(elem.second);
                this->policy_queue_.pop_back();
            } else {
                const StateID state_id =
                    this->state_id_map_->operator[](elem.second);
                auto& info = this->state_infos_->operator[](state_id);
                CAS_DEBUG_PRINT(
                    std::cout
                        << "(C&S) queue.top() = " << elem.second << " ~> "
                        << info.is_marked_open() << "|" << (info.is_solved())
                        << "|"
                        << (this->state_status_(elem.second).is_dead_end())
                        << " value="
                        << (this->state_status_(elem.second).get_value())
                        << std::endl;)
                if (info.is_marked_open()) {
                    this->policy_queue_.pop_back();
                } else if (info.is_solved()) {
                    const auto& state_info =
                        this->state_status_(state_id, info);
                    any_dead = any_dead || state_info.is_dead_end();
                    all_dead = all_dead && state_info.is_dead_end();
                    // if (this->dead_end_identification_level_
                    //     != DeadEndIdentificationLevel::Off) {
                    //     assert(
                    //         !this->state_status_(elem.second).is_dead_end()
                    //         || this->state_status_(elem.second)
                    //                .is_recognized_dead_end());
                    //     all_dead = all_dead
                    //         && this->state_status_(state_id, info)
                    //                .is_dead_end();
                    // }
                    this->policy_queue_.pop_back();
                } else {
                    info.mark_open();
                    elem.first = true;
                    this->statistics_.check_and_solve_bellman_backups++;
                    const bool value_changed = this->async_update(
                        elem.second, nullptr, &this->selected_transition_);
                    if (value_changed) {
                        epsilon_consistent = false;
                        CAS_DEBUG_PRINT(
                            std::cout
                                << "     => value has changed: "
                                << this->state_status_(elem.second).get_value()
                                << std::endl;)
                    } else {
                        if (this->selected_transition_.empty()) {
                            assert(
                                this->state_status_(elem.second).is_terminal());
                            if (this->check_goal_or_mark_dead_end(
                                    elem.second, state_id, info)) {
                                all_dead = false;
                            } else {
                                any_dead = true;
                            }
                            info.set_solved();
                            policy_queue_.pop_back();
                            CAS_DEBUG_PRINT(
                                std::cout << "     => marking as solved (dead= "
                                          << this->state_status_(elem.second)
                                                 .is_dead_end()
                                          << ")" << std::endl;)
                        } else if (epsilon_consistent) {
                            if (DualBounds::value && this->interval_comparison_
                                && !this->state_status_(elem.second)
                                        .bounds_equal()) {
                                mark_solved = false;
                            }
                            CAS_DEBUG_PRINT(std::cout << "  --> [";)
                            for (auto succ = this->selected_transition_.begin();
                                 succ != this->selected_transition_.end();
                                 ++succ) {
                                CAS_DEBUG_PRINT(
                                    std::cout
                                        << (succ
                                                    != this->selected_transition_
                                                           .begin()
                                                ? ", "
                                                : "")
                                        << succ->first << " ("
                                        << this->state_status_(succ->first)
                                               .value
                                        << ")";)
                                this->policy_queue_.emplace_back(
                                    false, succ->first);
                            }
                            CAS_DEBUG_PRINT(std::cout << "]" << std::endl;)
                        }
                    }
                    this->selected_transition_.clear();
                }
            }
        }

        if (listener_ != nullptr && epsilon_consistent && all_dead
            && any_dead) {
            this->last_check_and_solve_was_dead_ = true;
            std::pair<bool, bool> updated;
            for (auto it = this->visited_.begin(); it != this->visited_.end();
                 ++it) {
#if !defined(NDEBUG)
                {
                    const StateID id = this->state_id_map_->operator[](*it);
                    const StateInfo& info = this->state_infos_->operator[](id);
                    assert(!info.is_solved());
                    assert(info.is_marked_open());
                }
#endif
                CAS_DEBUG_PRINT(std::cout << "(C&S) checking dead-end " << *it
                                          << "..." << std::endl;)
                this->safe_async_update(
                    *it, *this->listener_, this->expansion_condition_, updated);
                epsilon_consistent = epsilon_consistent && !updated.second;
                if (updated.first) {
                    this->last_check_and_solve_was_dead_ = false;
                    break;
                }
                assert(this->state_status_(*it).is_dead_end());
            }
        }

        if (epsilon_consistent && mark_solved) {
            for (auto it = this->visited_.begin(); it != this->visited_.end();
                 ++it) {
                const StateID id = this->state_id_map_->operator[](*it);
                StateInfo& info = this->state_infos_->operator[](id);
                info.set_solved();
            }
        } else {
            for (auto it = visited_.begin(); it != visited_.end(); ++it) {
                statistics_.check_and_solve_bellman_backups++;
                this->async_update(*it);
                const StateID id = this->state_id_map_->operator[](*it);
                StateInfo& info = this->state_infos_->operator[](id);
                info.unmark();
            }
        }
        this->visited_.clear();

        return epsilon_consistent && mark_solved;
    }

    bool check_goal_or_mark_dead_end(
        const State& state,
        const StateID& state_id,
        StateInfo& info)
    {
        auto& state_info = this->state_status_(state_id, info);
        if (state_info.is_goal_state()) {
            return true;
        } else if (
            listener_ != nullptr && !state_info.is_recognized_dead_end()) {
            state_info.set_recognized_dead_end();
            listener_->operator()(state);
        }
        return false;
    }

    void setup_state_info_store(std::true_type)
    {
        state_infos_ = &this->get_state_info_store();
    }

    void setup_state_info_store(std::false_type)
    {
        state_infos_ = new storage::PerStateStorage<StateInfo>();
        statistics_.state_info_bytes = sizeof(StateInfo);
    }

    void cleanup_state_info_store(std::true_type) { }

    void cleanup_state_info_store(std::false_type) { delete (state_infos_); }

    const bool StopConsistent;
    storage::PerStateStorage<StateInfo>* state_infos_;
    TransitionSampler<State, Action>* sample_;
    StateListener<State, Action>* listener_;
    StateEvaluationFunction<State>* dead_end_identifier_;
    const DeadEndIdentificationLevel dead_end_identification_level_;
    const ExpandInDeadEndCheck expansion_condition_;
    typename HeuristicSearchBase::template StateStatusAccessor<StateInfo>
        state_status_;

    std::deque<State> current_trial_;
    Distribution<State> selected_transition_;
    std::deque<std::pair<bool, State>> policy_queue_;
    std::deque<State> visited_;

    Statistics statistics_;

    bool last_check_and_solve_was_dead_;
};

} // namespace internal

template<typename State, typename Action, typename B2>
using LRTDP = internal::LRTDP<heuristic_search_base::HeuristicSearchBase<
    State,
    Action,
    B2,
    std::true_type,
    internal::PerStateInformation>>;

template<typename State, typename Action, typename B2>
using LRTDPQ = internal::LRTDP<
    heuristic_search_base::
        HeuristicSearchBase<State, Action, B2, std::true_type>,
    internal::PerStateInformation<internal::EmptyStateInfo>>;

} // namespace lrtdp
} // namespace algorithms
} // namespace probabilistic

#undef CAS_DEBUG_PRINT
