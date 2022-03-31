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
    unsigned long long trials = 0;
    unsigned long long trial_bellman_backups = 0;
    unsigned long long check_and_solve_bellman_backups = 0;
    unsigned long long state_info_bytes = 0;

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
    uint8_t info = 0;
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
    using State = typename HeuristicSearchBase::State;
    using Action = typename HeuristicSearchBase::Action;
    using DualBounds = typename HeuristicSearchBase::DualBounds;

public:
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
        , sample_(succ_sampler)
    {
        this->setup_state_info_store();
    }

    virtual void solve(const State& state) override
    {
        this->initialize_report(state);
        const StateID state_id = this->get_state_id(state);

        for (;;) {
            StateInfoT& info = get_lrtdp_state_info(state_id);

            if (info.is_solved()) {
                break;
            }

            this->trial(state_id);
            this->statistics_.trials++;
            this->report(state_id);
        }
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
        HeuristicSearchBase::print_statistics(out);
    }

    virtual void reset_solver_state() override
    {
        this->state_infos_.reset(new storage::PerStateStorage<StateInfoT>());
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

            auto& state_info = get_lrtdp_state_info(state_id);
            if (state_info.is_solved()) {
                this->current_trial_.pop_back();
                break;
            }

            this->statistics_.trial_bellman_backups++;
            const bool value_changed = this->async_update(
                                               state_id,
                                               nullptr,
                                               &this->selected_transition_)
                                           .first;
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
            bool solved = this->check_and_solve(this->current_trial_.back());
            this->current_trial_.pop_back();

            if (this->last_check_and_solve_was_dead_ &&
                !this->current_trial_.empty()) {
                do {
                    if (!this->check_dead_end(this->current_trial_.back())) {
                        break;
                    }

                    solved = false;
                    this->current_trial_.pop_back();
                } while (!this->current_trial_.empty());
            }

            if (!solved) {
                break;
            }
        }

        this->current_trial_.clear();
    }

    bool check_and_solve(const StateID& init_state_id)
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

        {
            auto& init_info = get_state_info(init_state_id);

            if (init_info.is_solved()) {
                auto& stinfo = this->get_state_info(init_state_id, init_info);
                any_dead = all_dead = stinfo.is_dead_end();
            } else {
                init_info.mark_open();
                this->policy_queue_.emplace_back(false, init_state_id);
            }
        }

        while (!this->policy_queue_.empty()) {
            auto& elem = this->policy_queue_.back();
            const auto [flag, state_id] = elem;

            if (flag) {
                this->visited_.push_back(state_id);
                this->policy_queue_.pop_back();
                continue;
            }

            auto& info = get_lrtdp_state_info(state_id);
            assert(info.is_marked_open() && !info.is_solved());

            CAS_DEBUG_PRINT(std::cout << "(C&S) queue.top() = " << state_id
                                      << " ~> " << info.is_solved() << "|"
                                      << this->is_marked_dead_end(state_id)
                                      << " value=" << this->get_value(state_id)
                                      << std::endl;)

            elem.first = true;
            this->statistics_.check_and_solve_bellman_backups++;

            const bool value_changed = this->async_update(
                                               state_id,
                                               nullptr,
                                               &this->selected_transition_)
                                           .first;

            if (value_changed) {
                epsilon_consistent = false;
                CAS_DEBUG_PRINT(std::cout << "     => value has changed: "
                                          << this->get_value(state_id)
                                          << std::endl;)
            } else if (this->selected_transition_.empty()) {
                assert(this->get_state_info(state_id, info).is_terminal());

                if (this->check_goal_or_mark_dead_end(state_id, info)) {
                    all_dead = false;
                } else {
                    any_dead = true;
                }

                info.set_solved();
                policy_queue_.pop_back();
                CAS_DEBUG_PRINT(std::cout << "     => marking as solved (dead= "
                                          << this->is_marked_dead_end(state_id)
                                          << ")" << std::endl;)
            } else {
                if (this->do_bounds_disagree(state_id, info)) {
                    mark_solved = false;
                }

                for (const StateID& succ_id : selected_transition_.elements()) {
                    auto& succ_info = get_state_info(succ_id);
                    if (succ_info.is_solved()) {
                        auto& succsi = this->get_state_info(succ_id, succ_info);
                        any_dead = any_dead || succsi.is_dead_end();
                        all_dead = all_dead && succsi.is_dead_end();
                    } else if (!succ_info.is_marked_open()) {
                        succ_info.mark_open();
                        this->policy_queue_.emplace_back(false, succ_id);
                    }
                }
            }

            this->selected_transition_.clear();
        }

        if (epsilon_consistent && all_dead && any_dead &&
            this->is_dead_end_learning_enabled()) {
            this->last_check_and_solve_was_dead_ = true;
        }

        if (epsilon_consistent && mark_solved) {
            for (const StateID& sid : this->visited_) {
                get_lrtdp_state_info(sid).set_solved();
            }
        } else {
            for (const StateID& sid : this->visited_) {
                statistics_.check_and_solve_bellman_backups++;
                this->async_update(sid);
                get_lrtdp_state_info(sid).unmark();
            }
        }

        this->visited_.clear();

        return epsilon_consistent && mark_solved;
    }

    bool check_and_solve_original(const StateID& init_state_id)
    {
        this->last_check_and_solve_was_dead_ = false;

        bool rv = true;

        get_lrtdp_state_info(init_state_id).mark_open();
        this->policy_queue_.emplace_back(false, init_state_id);

        do {
            const StateID stateid = this->policy_queue_.back().second;
            this->policy_queue_.pop_back();
            this->visited_.push_back(stateid);

            auto& info = get_lrtdp_state_info(stateid);
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
            } else if (rv && !this->selected_transition_.empty()) {
                for (const StateID& succid : selected_transition_.elements()) {
                    auto& succ_info = get_lrtdp_state_info(succid);
                    if (!succ_info.is_solved() && !succ_info.is_marked_open()) {
                        succ_info.mark_open();
                        this->policy_queue_.emplace_back(false, succid);
                    }
                }
            }
            this->selected_transition_.clear();
        } while (!this->policy_queue_.empty());

        if (rv) {
            while (!this->visited_.empty()) {
                const StateID sid = this->visited_.back();
                auto& info = get_lrtdp_state_info(sid);
                info.unmark();
                info.set_solved();
                this->visited_.pop_back();
            }
        } else {
            while (!this->visited_.empty()) {
                const StateID sid = this->visited_.back();
                auto& info = get_lrtdp_state_info(sid);
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
        }

        this->notify_dead_end(state_id, state_info);
        return false;
    }

    StateInfoT& get_lrtdp_state_info(const StateID& sid)
    {
        using HSBInfo = typename HeuristicSearchBase::StateInfo;

        if constexpr (std::is_same_v<StateInfoT, HSBInfo>) {
            return this->get_state_info_store()[sid];
        } else {
            return (*state_infos_)[sid];
        }
    }

    void setup_state_info_store()
    {
        using HSBInfo = typename HeuristicSearchBase::StateInfo;

        if constexpr (!std::is_same_v<StateInfoT, HSBInfo>) {
            state_infos_.reset(new storage::PerStateStorage<StateInfoT>());
            statistics_.state_info_bytes = sizeof(StateInfoT);
        }
    }

    const TrialTerminationCondition StopConsistent;
    std::unique_ptr<storage::PerStateStorage<StateInfoT>> state_infos_;
    TransitionSampler<Action>* sample_;

    std::vector<StateID> current_trial_;
    Distribution<StateID> selected_transition_;
    std::vector<std::pair<bool, StateID>> policy_queue_;
    std::vector<StateID> visited_;

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
template <typename StateT, typename ActionT, typename B2>
struct LRTDP<StateT, ActionT, B2, std::false_type>
    : public internal::LRTDP<heuristic_search::HeuristicSearchBase<
          StateT,
          ActionT,
          B2,
          std::true_type,
          internal::PerStateInformation>> {
    using internal::LRTDP<heuristic_search::HeuristicSearchBase<
        StateT,
        ActionT,
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
template <typename StateT, typename ActionT, typename B2>
struct LRTDP<StateT, ActionT, B2, std::true_type>
    : public internal::LRTDP<
          heuristic_search::
              HeuristicSearchBase<StateT, ActionT, B2, std::true_type>,
          internal::PerStateInformation<internal::EmptyStateInfo>> {
    using internal::LRTDP<
        heuristic_search::
            HeuristicSearchBase<StateT, ActionT, B2, std::true_type>,
        internal::PerStateInformation<internal::EmptyStateInfo>>::LRTDP;
};

} // namespace lrtdp
} // namespace engines
} // namespace probabilistic

#undef CAS_DEBUG_PRINT

#endif // __LRTDP_H__