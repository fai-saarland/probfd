#ifndef MDPS_ENGINES_LRTDP_H
#define MDPS_ENGINES_LRTDP_H

#include "../engine_interfaces/transition_sampler.h"
#include "heuristic_search_base.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>

#define CAS_DEBUG_PRINT(x)

namespace probabilistic {
namespace engines {

/// Namespace dedicated to labelled real-time dynamic programming (LRTDP).
namespace lrtdp {

enum class TrialTerminationCondition {
    Disabled = 0,
    Consistent = 1,
    Inconsistent = 2,
};

namespace internal {

struct Statistics {
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

    void print(std::ostream& out) const
    {
        out << "  Additional per state information: " << state_info_bytes
            << " bytes" << std::endl;
        out << "  Trials: " << trials << std::endl;
        out << "  Bellman backups (trials): " << trial_bellman_backups
            << std::endl;
        out << "  Bellman backups (check&solved): "
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

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr const uint8_t MARKED_OPEN = (1 << StateInfo::BITS);
    // static constexpr const uint8_t MARKED_TRIAL = (2 << StateInfo::BITS);
    static constexpr const uint8_t SOLVED = (3 << StateInfo::BITS);
    static constexpr const uint8_t BITS = StateInfo::BITS + 2;
    static constexpr const uint8_t MASK = (3 << StateInfo::BITS);

    bool is_marked_open() const { return (this->info & MASK) == MARKED_OPEN; }
    // bool is_marked_trial() const
    // {
    //     return (this->info & MASK) == MARKED_TRIAL;
    // }
    bool is_solved() const { return (this->info & MASK) == SOLVED; }
    void mark_open()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK) | MARKED_OPEN;
    }
    // void mark_trial()
    // {
    //     assert(!is_solved());
    //     this->info = (this->info & ~MASK) | MARKED_TRIAL;
    // }
    void unmark()
    {
        assert(!is_solved());
        this->info = this->info & ~MASK;
    }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
};

template <
    typename HeuristicSearchBase,
    typename StateInfoT = typename HeuristicSearchBase::StateInfo>
class LRTDP : public HeuristicSearchBase {
public:
    using State = typename HeuristicSearchBase::State;
    using Action = typename HeuristicSearchBase::Action;
    using DualBounds = typename HeuristicSearchBase::DualBounds;

    template <typename... Args>
    LRTDP(
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
        TrialTerminationCondition stop_consistent,
        TransitionSampler<Action>* succ_sampler)
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
        , StopConsistent(stop_consistent)
        , state_infos_(nullptr)
        , sample_(succ_sampler)
    {
        this->setup_state_info_store(
            typename std::is_same<
                StateInfoT,
                typename HeuristicSearchBase::StateInfo>::type());
    }

    ~LRTDP()
    {
        this->cleanup_state_info_store(
            typename std::is_same<
                StateInfoT,
                typename HeuristicSearchBase::StateInfo>::type());
    }

    virtual void solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID state_id = this->get_state_id(state);
        do {
            StateInfoT& info = this->state_infos_->operator[](state_id);
            if (info.is_solved()) {
                break;
            }
            this->trial(state_id);
            this->statistics_.trials++;
            this->report(state_id);
        } while (true);
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
        HeuristicSearchBase::print_statistics(out);
    }

    virtual void reset_solver_state() override
    {
        delete (this->state_infos_);
        this->state_infos_ = new storage::PerStateStorage<StateInfoT>();
    }

protected:
    virtual void setup_custom_reports(const State&) override
    {
        this->statistics_.register_report(*this->report_);
    }

private:
    void trial(const StateID& initial_state)
    {
        assert(
            this->current_trial_.empty() && this->selected_transition_.empty());
        this->current_trial_.push_back(initial_state);
        // std::cout << "trial = [";
        while (true) {
            const StateID state_id = this->current_trial_.back();
            // std::cout << " " << state_id;
            auto& state_info = state_infos_->operator[](state_id);
            if (state_info.is_solved()) {
                this->current_trial_.pop_back();
                break;
            }
            this->statistics_.trial_bellman_backups++;
            const bool value_changed = this->async_update(
                state_id,
                nullptr,
                &this->selected_transition_,
                nullptr);
            if (this->selected_transition_.empty()) {
                // terminal
                assert(
                    this->get_state_info(state_id, state_info).is_terminal());
                this->check_goal_or_mark_dead_end(state_id, state_info);
                state_info.set_solved();
                this->current_trial_.pop_back();
                break;
            }
            // state_info.mark_trial();
            assert(!this->get_state_info(state_id, state_info).is_terminal());
            if ((StopConsistent == TrialTerminationCondition::Consistent &&
                 !value_changed) ||
                (StopConsistent == TrialTerminationCondition::Inconsistent &&
                 value_changed)) {
                this->selected_transition_.clear();
                break;
            }
            this->current_trial_.push_back(sample_->operator()(
                state_id,
                this->get_policy(state_id),
                this->selected_transition_));
            this->selected_transition_.clear();
        }
        // std::cout << " ]" << std::endl;
        this->last_check_and_solve_was_dead_ = true;
        while (!this->current_trial_.empty()) {
            bool solved = this->check_and_solve();
            this->current_trial_.pop_back();
            if (this->last_check_and_solve_was_dead_ &&
                !this->current_trial_.empty()) {
                do {
                    if (this->check_dead_end(this->current_trial_.back())) {
                        solved = false;
                        this->current_trial_.pop_back();
                    } else {
                        break;
                    }
                } while (!this->current_trial_.empty());
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
        bool all_dead = true;
        // this->last_check_and_solve_was_dead_
        //    && (!StopConsistent
        //        || this->has_dead_end_value(this->current_trial_.back()));
        bool any_dead = false;
        this->last_check_and_solve_was_dead_ = false;

        this->policy_queue_.emplace_back(false, this->current_trial_.back());
        while (!this->policy_queue_.empty()) {
            auto& elem = this->policy_queue_.back();
            if (elem.first) {
                this->visited_.push_back(elem.second);
                this->policy_queue_.pop_back();
            } else {
                const StateID state_id = elem.second;
                auto& info = this->state_infos_->operator[](state_id);
                CAS_DEBUG_PRINT(
                    std::cout
                        << "(C&S) queue.top() = " << elem.second << " ~> "
                        << info.is_marked_open() << "|" << (info.is_solved())
                        << "|"
                        << (this->get_state_info(elem.second).is_dead_end())
                        << " value="
                        << (this->get_state_info(elem.second).get_value())
                        << std::endl;)
                if (info.is_marked_open()) {
                    this->policy_queue_.pop_back();
                } else if (info.is_solved()) {
                    const auto& state_info =
                        this->get_state_info(state_id, info);
                    any_dead = any_dead || state_info.is_dead_end();
                    all_dead = all_dead && state_info.is_dead_end();
                    // if (this->dead_end_identification_level_
                    //     != DeadEndIdentificationLevel::Off) {
                    //     assert(
                    //         !this->get_state_info(elem.second).is_dead_end()
                    //         || this->get_state_info(elem.second)
                    //                .is_recognized_dead_end());
                    //     all_dead = all_dead
                    //         && this->get_state_info(state_id, info)
                    //                .is_dead_end();
                    // }
                    this->policy_queue_.pop_back();
                } else {
                    info.mark_open();
                    elem.first = true;
                    this->statistics_.check_and_solve_bellman_backups++;
                    const bool value_changed = this->async_update(
                        elem.second,
                        nullptr,
                        &this->selected_transition_,
                        nullptr);
                    if (value_changed) {
                        epsilon_consistent = false;
                        CAS_DEBUG_PRINT(
                            std::cout
                                << "     => value has changed: "
                                << this->get_state_info(elem.second).get_value()
                                << std::endl;)
                    } else {
                        if (this->selected_transition_.empty()) {
                            assert(this->get_state_info(elem.second)
                                       .is_terminal());
                            if (this->check_goal_or_mark_dead_end(
                                    elem.second,
                                    info)) {
                                all_dead = false;
                            } else {
                                any_dead = true;
                            }
                            info.set_solved();
                            policy_queue_.pop_back();
                            CAS_DEBUG_PRINT(
                                std::cout << "     => marking as solved (dead= "
                                          << this->get_state_info(elem.second)
                                                 .is_dead_end()
                                          << ")" << std::endl;)
                        } else {
                            if (this->do_bounds_disagree(state_id, info)) {
                                mark_solved = false;
                            }
                            CAS_DEBUG_PRINT(std::cout << "  --> [";)
                            for (auto succ = this->selected_transition_.begin();
                                 succ != this->selected_transition_.end();
                                 ++succ) {
                                CAS_DEBUG_PRINT(
                                    std::cout
                                        << (succ != this->selected_transition_
                                                        .begin()
                                                ? ", "
                                                : "")
                                        << succ->first << " ("
                                        << this->get_state_info(succ->first)
                                               .value
                                        << ")";)
                                this->policy_queue_.emplace_back(
                                    false,
                                    succ->first);
                            }
                            CAS_DEBUG_PRINT(std::cout << "]" << std::endl;)
                        }
                    }
                    this->selected_transition_.clear();
                }
            }
        }

        if (epsilon_consistent && all_dead && any_dead &&
            this->is_dead_end_learning_enabled()) {
            this->last_check_and_solve_was_dead_ = true;
        }

        if (epsilon_consistent && mark_solved) {
            for (auto it = this->visited_.begin(); it != this->visited_.end();
                 ++it) {
                StateInfoT& info = this->state_infos_->operator[](*it);
                info.set_solved();
            }
        } else {
            for (auto it = visited_.begin(); it != visited_.end(); ++it) {
                statistics_.check_and_solve_bellman_backups++;
                this->async_update(*it);
                StateInfoT& info = this->state_infos_->operator[](*it);
                info.unmark();
            }
        }
        this->visited_.clear();

        return epsilon_consistent && mark_solved;
    }

    bool check_and_solve_original()
    {
        this->last_check_and_solve_was_dead_ = false;

        bool rv = true;
        {
            const StateID stateid = this->current_trial_.back();
            auto& info = this->state_infos_->operator[](stateid);
            info.mark_open();
        }
        this->policy_queue_.emplace_back(false, this->current_trial_.back());
        do {
            const StateID stateid = this->policy_queue_.back().second;
            this->policy_queue_.pop_back();
            this->visited_.push_back(stateid);

            auto& info = this->state_infos_->operator[](stateid);
            if (info.is_solved()) {
                continue;
            }
            this->statistics_.check_and_solve_bellman_backups++;
            const bool value_changed = this->async_update(
                stateid,
                nullptr,
                &this->selected_transition_);
            if (value_changed) {
                rv = false;
            } else if (rv || true) {
                if (!this->selected_transition_.empty()) {
                    for (auto it = this->selected_transition_.begin();
                         it != this->selected_transition_.end();
                         ++it) {
                        const StateID succid = it->first;
                        auto& succ_info =
                            this->state_infos_->operator[](succid);
                        if (!succ_info.is_solved() &&
                            !succ_info.is_marked_open()) {
                            succ_info.mark_open();
                            this->policy_queue_.emplace_back(false, it->first);
                        }
                    }
                }
            }
            this->selected_transition_.clear();
        } while (!this->policy_queue_.empty());

        if (rv) {
            while (!this->visited_.empty()) {
                const StateID sid = this->visited_.back();
                auto& info = this->state_infos_->operator[](sid);
                info.unmark();
                info.set_solved();
                this->visited_.pop_back();
            }
        } else {
            while (!this->visited_.empty()) {
                const StateID sid = this->visited_.back();
                auto& info = this->state_infos_->operator[](sid);
                info.unmark();
                this->async_update(sid);
                this->visited_.pop_back();
                this->statistics_.check_and_solve_bellman_backups++;
            }
        }
        return rv;
    }

    bool check_goal_or_mark_dead_end(const StateID& state_id, StateInfoT& info)
    {
        auto& state_info = this->get_state_info(state_id, info);
        if (state_info.is_goal_state()) {
            return true;
        } else {
            this->notify_dead_end(state_id, state_info);
        }
        return false;
    }

    void setup_state_info_store(std::true_type)
    {
        state_infos_ = &this->get_state_info_store();
    }

    void setup_state_info_store(std::false_type)
    {
        state_infos_ = new storage::PerStateStorage<StateInfoT>();
        statistics_.state_info_bytes = sizeof(StateInfoT);
    }

    void cleanup_state_info_store(std::true_type) {}

    void cleanup_state_info_store(std::false_type) { delete (state_infos_); }

    const TrialTerminationCondition StopConsistent;
    storage::PerStateStorage<StateInfoT>* state_infos_;
    TransitionSampler<Action>* sample_;

    std::deque<StateID> current_trial_;
    Distribution<StateID> selected_transition_;
    std::deque<std::pair<bool, StateID>> policy_queue_;
    std::deque<StateID> visited_;

    Statistics statistics_;

    bool last_check_and_solve_was_dead_;
};

} // namespace internal

/**
 * @brief Template base of labelled real-time dynamic programming (LRTDP)
 * \cite bonet:geffner:icaps-03 with integrated FRET support.
 */
template <typename State, typename Action, typename B2, typename Fret>
struct LRTDP;

/**
 * @brief Implementation of LRTDP with FRET disabled.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam B2 - Whether bounded value iteration is used.
 */
template <typename State, typename Action, typename B2>
struct LRTDP<State, Action, B2, std::false_type>
    : public internal::LRTDP<heuristic_search::HeuristicSearchBase<
          State,
          Action,
          B2,
          std::true_type,
          internal::PerStateInformation>> {
    using internal::LRTDP<heuristic_search::HeuristicSearchBase<
        State,
        Action,
        B2,
        std::true_type,
        internal::PerStateInformation>>::LRTDP;
};

/**
 * @brief Implementation of LRTDP with FRET enabled.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam B2 - Whether bounded value iteration is used.
 */
template <typename State, typename Action, typename B2>
struct LRTDP<State, Action, B2, std::true_type>
    : public internal::LRTDP<
          heuristic_search::
              HeuristicSearchBase<State, Action, B2, std::true_type>,
          internal::PerStateInformation<internal::EmptyStateInfo>> {
    using internal::LRTDP<
        heuristic_search::
            HeuristicSearchBase<State, Action, B2, std::true_type>,
        internal::PerStateInformation<internal::EmptyStateInfo>>::LRTDP;
};

} // namespace lrtdp
} // namespace engines
} // namespace probabilistic

#undef CAS_DEBUG_PRINT

#endif // __LRTDP_H__