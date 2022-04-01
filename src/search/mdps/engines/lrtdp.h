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

// When FRET is enabled, store the LRTDP-specific state information seperately
// so it can be easily reset between FRET iterations. Otherwise, store the
// LRTDP-specific state information with the basic state information.
template <typename State, typename Action, typename DualBounds, bool Fret>
using LRTDPBase = std::conditional_t<
    Fret,
    heuristic_search::
        HeuristicSearchBase<State, Action, DualBounds, std::true_type>,
    heuristic_search::HeuristicSearchBase<
        State,
        Action,
        DualBounds,
        std::true_type,
        internal::PerStateInformation>>;

template <typename State, typename Action, typename DualBounds, bool Fret>
using LRTDPStateInfo = std::conditional_t<
    Fret,
    internal::PerStateInformation<internal::EmptyStateInfo>,
    typename LRTDPBase<State, Action, DualBounds, Fret>::StateInfo>;

} // namespace internal

/**
 * @brief Implements the labelled real-time dynamic programming (LRTDP)
 * algorithm \cite bonet:geffner:icaps-03.
 *
 * This algorithm extends the trial-based real-time dynamic programming (RTDP)
 * algorithm \cite barto:etal:ai-95. A trial is a simulation of the greedy
 * policy which starts in the initial state and runs until a goal state
 * is reached, which results in a path from the initial to the goal state.
 * All states along this path are then asynchronously updated in a forward
 * manner. In the limit, repeated trials will lead to an optimal state value for
 * the initial state.
 *
 * LRTDP extends RTDP with a labelling procedure that marks parts of the state
 * space where the value function is epsilon-consistent. This labelling
 * procedure traverses the greedy policy graph starting from a specific state
 * and updates every reachable state it encounters once. If none of the
 * reachable states changed its value by more than epsilon, all states are
 * labelled as solved as they are epsilon-consistent. Solved states are treated
 * as stopping points for future trials. The labelling procedure is called on
 * every state of a trial, in reverse order. If the initial state is marked as
 * solved, the algorithm terminates.
 *
 * \remark This implementation can also be used with interval value bounds.
 * Here, epsilon-consistency of the value updates can be replaced with
 * epsilon-closeness of the resulting state interval bounds.
 *
 * \remark For MaxProb problems, trials may get stuck in cycles. To deal with
 * this, alternative trial termination conditions can be selected, which ensure
 * that LRTDP works even in presence of dead-ends.
 *
 * @tparam State - The state type of the MDP model.
 * @tparam Action - The action type of the MDP model.
 * @tparam DualBounds - Whether intervals or real values are used as state
 * values.
 * @tparam Fret - Specifies whether the algorithm should be usable within FRET.
 */
template <typename State, typename Action, typename DualBounds, typename Fret>
class LRTDP
    : public internal::LRTDPBase<State, Action, DualBounds, Fret::value> {
    using HeuristicSearchBase =
        internal::LRTDPBase<State, Action, DualBounds, Fret::value>;
    using StateInfoT =
        internal::LRTDPStateInfo<State, Action, DualBounds, Fret::value>;

    using Statistics = internal::Statistics;

public:
    /**
     * @brief Constructs an LRTDP solver object.
     */
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

    /**
     * @copydoc MDPEngineInterface::solve(const State& state)
     */
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

    /**
     * @copydoc MDPEngineInterface::print_statistics(std::ostream& out)
     */
    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
        HeuristicSearchBase::print_statistics(out);
    }

    /**
     * @copydoc MDPEngineInterface::reset_solver_state()
     */
    virtual void reset_solver_state() override
    {
        using HSBInfo = typename HeuristicSearchBase::StateInfo;

        if constexpr (!std::is_same_v<StateInfoT, HSBInfo>) {
            this->state_infos_.reset(
                new storage::PerStateStorage<StateInfoT>());
        }
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
            auto& init_info = get_lrtdp_state_info(init_state_id);

            if (init_info.is_solved()) {
                auto& stinfo = this->get_state_info(init_state_id, init_info);
                any_dead = all_dead = stinfo.is_dead_end();
            } else {
                init_info.mark_open();
                this->policy_queue_.emplace_back(init_state_id);
            }
        }

        while (!this->policy_queue_.empty()) {
            const auto state_id = this->policy_queue_.back();
            policy_queue_.pop_back();

            auto& info = get_lrtdp_state_info(state_id);
            assert(info.is_marked_open() && !info.is_solved());

            CAS_DEBUG_PRINT(std::cout << "(C&S) queue.top() = " << state_id
                                      << " ~> "
                                      << this->is_marked_dead_end(state_id)
                                      << " value=" << this->get_value(state_id)
                                      << std::endl;)

            this->statistics_.check_and_solve_bellman_backups++;

            const bool value_changed = this->async_update(
                                               state_id,
                                               nullptr,
                                               &this->selected_transition_)
                                           .first;

            if (value_changed) {
                epsilon_consistent = false;
                this->visited_.push_front(state_id);

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

                CAS_DEBUG_PRINT(std::cout << "     => marking as solved (dead= "
                                          << this->is_marked_dead_end(state_id)
                                          << ")" << std::endl;)
            } else {
                this->visited_.push_front(state_id);

                if (this->do_bounds_disagree(state_id, info)) {
                    mark_solved = false;
                }

                for (const StateID& succ_id : selected_transition_.elements()) {
                    auto& succ_info = get_lrtdp_state_info(succ_id);
                    if (succ_info.is_solved()) {
                        auto& succsi = this->get_state_info(succ_id, succ_info);
                        any_dead = any_dead || succsi.is_dead_end();
                        all_dead = all_dead && succsi.is_dead_end();
                    } else if (!succ_info.is_marked_open()) {
                        succ_info.mark_open();
                        this->policy_queue_.emplace_back(succ_id);
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
        this->policy_queue_.emplace_back(init_state_id);

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
                        this->policy_queue_.emplace_back(succid);
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
    std::vector<StateID> policy_queue_;
    std::deque<StateID> visited_;

    Statistics statistics_;

    bool last_check_and_solve_was_dead_;
};

} // namespace lrtdp
} // namespace engines
} // namespace probabilistic

#undef CAS_DEBUG_PRINT

#endif // __LRTDP_H__