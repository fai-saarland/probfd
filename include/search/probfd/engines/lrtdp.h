#ifndef PROBFD_ENGINES_LRTDP_H
#define PROBFD_ENGINES_LRTDP_H

#include "probfd/engines/heuristic_search_base.h"

#include "probfd/engine_interfaces/successor_sampler.h"

#include "utils/countdown_timer.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>

namespace probfd {
namespace engines {

/// Namespace dedicated to labelled real-time dynamic programming (LRTDP).
namespace lrtdp {

/**
 * @brief Enumeration type specifying the termination condition for trials
 * sampled during LRTDP.
 */
enum class TrialTerminationCondition {
    /**
     * Runs the trial until a terminal state is encountered.
     */
    TERMINAL,
    /**
     * Runs the trial until a state with epsilon-consistent Bellman update is
     * encountered.
     */
    CONSISTENT,
    /**
     * Runs the trial until a state with epsilon-inconsistent Bellman update is
     * encountered.
     */
    INCONSISTENT,
    /**
     * Runs the trial until a state is re-visited.
     */
    REVISITED
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
};

struct EmptyStateInfo {
    static constexpr const uint8_t BITS = 0;
    uint8_t info = 0;
};

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr uint8_t MARKED_OPEN = (1 << StateInfo::BITS);
    static constexpr uint8_t MARKED_TRIAL = (2 << StateInfo::BITS);
    static constexpr uint8_t SOLVED = (4 << StateInfo::BITS);
    static constexpr uint8_t BITS = StateInfo::BITS + 3;
    static constexpr uint8_t MASK = (7 << StateInfo::BITS);

    bool is_marked_open() const { return this->info & MARKED_OPEN; }

    bool is_marked_trial() const { return this->info & MARKED_TRIAL; }

    bool is_solved() const { return this->info & SOLVED; }

    void mark_open()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK) | MARKED_OPEN;
    }

    void mark_trial()
    {
        assert(!is_solved());
        this->info = (this->info & ~MASK) | MARKED_TRIAL;
    }

    void mark_solved() { this->info = (this->info & ~MASK) | SOLVED; }

    void unmark_trial()
    {
        assert(!is_solved());
        this->info = this->info & ~MARKED_TRIAL;
    }

    void unmark()
    {
        assert(!is_solved());
        this->info = this->info & ~MASK;
    }
};

// When FRET is enabled, store the LRTDP-specific state information seperately
// so it can be easily reset between FRET iterations. Otherwise, store the
// LRTDP-specific state information with the basic state information.
template <typename State, typename Action, bool UseInterval, bool Fret>
using LRTDPBase = std::conditional_t<
    Fret,
    heuristic_search::HeuristicSearchBase<State, Action, UseInterval, true>,
    heuristic_search::HeuristicSearchBase<
        State,
        Action,
        UseInterval,
        true,
        internal::PerStateInformation>>;

template <typename State, typename Action, bool UseInterval, bool Fret>
using LRTDPStateInfo = std::conditional_t<
    Fret,
    internal::PerStateInformation<internal::EmptyStateInfo>,
    typename LRTDPBase<State, Action, UseInterval, Fret>::StateInfo>;

} // namespace internal

/**
 * @brief Implements the labelled real-time dynamic programming (LRTDP)
 * algorithm \cite bonet:geffner:icaps-03.
 *
 * This algorithm extends the trial-based real-time dynamic programming (RTDP)
 * algorithm \cite barto:etal:ai-95. The algorithm iteratively generates
 * randomly simulated traces (trials) of the greedy policy, starting from the
 * initial state. For each encountered state, an asynchronous Bellman update
 * is performed from first to last encountered state, and the greedy policy is
 * updated. LRTDP additionally labels states as solved if they can only reach
 * epsilon-consistent states. Solved states are treated as stopping points for
 * future trials. The algorithm terminates when the initial state is marked as
 * solved.
 *
 * The method to generate the trials can be configured. The interface
 * \ref engine_interfaces::SuccessorSampler describes how successor
 * states in the greedy policy graph are sampled when generating a trial.
 * Additionally, the enumeration \ref TrialTerminationCondition specifies when a
 * trial is stopped.
 *
 * This implementation also supports value intervals. In this case, the
 * labelling procedure marks a state as solved if its value interval is
 * sufficiently small.
 *
 * @tparam State - The state type of the MDP model.
 * @tparam Action - The action type of the MDP model.
 * @tparam UseInterval - Whether intervals or real values are used as state
 * values.
 * @tparam Fret - Specifies whether the algorithm should be usable within FRET.
 */
template <typename State, typename Action, bool UseInterval, bool Fret>
class LRTDP : public internal::LRTDPBase<State, Action, UseInterval, Fret> {
    using HeuristicSearchBase =
        internal::LRTDPBase<State, Action, UseInterval, Fret>;
    using StateInfoT =
        internal::LRTDPStateInfo<State, Action, UseInterval, Fret>;
    using Statistics = internal::Statistics;

    const TrialTerminationCondition StopConsistent;
    engine_interfaces::SuccessorSampler<Action>* sample_;

    storage::PerStateStorage<StateInfoT> state_infos_;

    std::vector<StateID> current_trial_;
    Distribution<StateID> selected_transition_;
    std::vector<StateID> policy_queue_;
    std::deque<StateID> visited_;

    Statistics statistics_;

public:
    /**
     * @brief Constructs an LRTDP solver object.
     */
    LRTDP(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<State, Action>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        TrialTerminationCondition stop_consistent,
        engine_interfaces::SuccessorSampler<Action>* succ_sampler)
        : HeuristicSearchBase(
              state_space,
              cost_function,
              value_init,
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison)
        , StopConsistent(stop_consistent)
        , sample_(succ_sampler)
    {
        using HSBInfo = typename HeuristicSearchBase::StateInfo;

        if constexpr (!std::is_same_v<StateInfoT, HSBInfo>) {
            statistics_.state_info_bytes = sizeof(StateInfoT);
        }
    }

    void reset_search_state() override { state_infos_.clear(); }

protected:
    Interval do_solve(param_type<State> state, double max_time) override
    {
        utils::CountdownTimer timer(max_time);

        const StateID state_id = this->get_state_id(state);

        for (;;) {
            StateInfoT& info = get_lrtdp_state_info(state_id);

            if (info.is_solved()) {
                break;
            }

            this->trial(state_id, timer);
            this->statistics_.trials++;
            this->print_progress();
        }

        return this->lookup_bounds(state_id);
    }

    void print_additional_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

protected:
    void setup_custom_reports(param_type<State>) override
    {
        this->report_->register_print(
            [&](std::ostream& out) { out << "trials=" << statistics_.trials; });
    }

private:
    void trial(StateID initial_state, utils::CountdownTimer& timer)
    {
        using enum TrialTerminationCondition;

        ClearGuard guard(this->current_trial_);

        this->current_trial_.push_back(initial_state);

        for (;;) {
            timer.throw_if_expired();

            const StateID state_id = this->current_trial_.back();

            auto& state_info = get_lrtdp_state_info(state_id);
            if (state_info.is_solved()) {
                this->current_trial_.pop_back();
                break;
            }

            ClearGuard guard(this->selected_transition_);

            this->statistics_.trial_bellman_backups++;
            const bool value_changed =
                this->async_update(state_id, &this->selected_transition_)
                    .value_changed;
            if (this->selected_transition_.empty()) {
                auto& base_info = this->get_state_info(state_id, state_info);
                // terminal
                assert(base_info.is_terminal());
                this->notify_dead_end_ifnot_goal(base_info);
                state_info.mark_solved();
                this->current_trial_.pop_back();
                break;
            }

            // state_info.mark_trial();
            assert(!this->get_state_info(state_id, state_info).is_terminal());

            if ((StopConsistent == CONSISTENT && !value_changed) ||
                (StopConsistent == INCONSISTENT && value_changed) ||
                (StopConsistent == REVISITED && state_info.is_marked_trial())) {
                break;
            }

            if (StopConsistent == REVISITED) {
                state_info.mark_trial();
            }

            this->current_trial_.push_back(this->sample_state(
                *sample_,
                state_id,
                this->selected_transition_));
        }

        if (StopConsistent == REVISITED) {
            for (const StateID state : current_trial_) {
                auto& info = this->get_lrtdp_state_info(state);
                assert(info.is_marked_trial());
                info.unmark_trial();
            }
        }

        do {
            timer.throw_if_expired();

            if (!this->check_and_solve(this->current_trial_.back(), timer)) {
                break;
            }

            this->current_trial_.pop_back();
        } while (!this->current_trial_.empty());
    }

    bool check_and_solve(StateID init_state_id, utils::CountdownTimer& timer)
    {
        assert(!this->current_trial_.empty());
        assert(this->policy_queue_.empty());

        ClearGuard guard(this->visited_);

        bool mark_solved = true;
        bool epsilon_consistent = true;

        {
            auto& init_info = get_lrtdp_state_info(init_state_id);

            if (!init_info.is_solved()) {
                init_info.mark_open();
                this->policy_queue_.emplace_back(init_state_id);
            }
        }

        while (!this->policy_queue_.empty()) {
            timer.throw_if_expired();

            ClearGuard guard(this->selected_transition_);

            const auto state_id = this->policy_queue_.back();
            policy_queue_.pop_back();

            auto& info = get_lrtdp_state_info(state_id);
            assert(info.is_marked_open() && !info.is_solved());

            this->statistics_.check_and_solve_bellman_backups++;

            const bool value_changed =
                this->async_update(state_id, &this->selected_transition_)
                    .value_changed;

            if (value_changed) {
                epsilon_consistent = false;
                this->visited_.push_front(state_id);
            } else if (this->selected_transition_.empty()) {
                auto& base_info = this->get_state_info(state_id, info);
                assert(base_info.is_terminal());

                this->notify_dead_end_ifnot_goal(base_info);
                info.mark_solved();
            } else {
                this->visited_.push_front(state_id);

                if constexpr (UseInterval) {
                    if (this->check_interval_comparison() &&
                        !this->get_state_info(state_id, info).bounds_agree()) {
                        mark_solved = false;
                    }
                }

                for (StateID succ_id : selected_transition_.support()) {
                    auto& succ_info = get_lrtdp_state_info(succ_id);
                    if (!succ_info.is_solved() && !succ_info.is_marked_open()) {
                        succ_info.mark_open();
                        this->policy_queue_.emplace_back(succ_id);
                    }
                }
            }
        }

        if (epsilon_consistent && mark_solved) {
            for (StateID sid : this->visited_) {
                get_lrtdp_state_info(sid).mark_solved();
            }
        } else {
            for (StateID sid : this->visited_) {
                statistics_.check_and_solve_bellman_backups++;
                this->async_update(sid);
                get_lrtdp_state_info(sid).unmark();
            }
        }

        return epsilon_consistent && mark_solved;
    }

    StateInfoT& get_lrtdp_state_info(StateID sid)
    {
        using HSBInfo = typename HeuristicSearchBase::StateInfo;

        if constexpr (std::is_same_v<StateInfoT, HSBInfo>) {
            return this->get_state_info(sid);
        } else {
            return state_infos_[sid];
        }
    }

    /*
    bool check_and_solve_original(StateID init_state_id)
    {
        bool rv = true;

        get_lrtdp_state_info(init_state_id).mark_open();
        this->policy_queue_.emplace_back(init_state_id);

        do {
            const StateID stateid = this->policy_queue_.back();
            this->policy_queue_.pop_back();
            this->visited_.push_back(stateid);

            auto& info = get_lrtdp_state_info(stateid);
            if (info.is_solved()) {
                continue;
            }

            ClearGuard guard(this->selected_transition_);

            this->statistics_.check_and_solve_bellman_backups++;
            const bool value_changed =
                this->async_update(stateid, &this->selected_transition_);

            if (value_changed) {
                rv = false;
                continue;
            }

            if (rv && !this->selected_transition_.empty()) {
                for (StateID succid : selected_transition_.support()) {
                    auto& succ_info = get_lrtdp_state_info(succid);
                    if (!succ_info.is_solved() && !succ_info.is_marked_open()) {
                        succ_info.mark_open();
                        this->policy_queue_.emplace_back(succid);
                    }
                }
            }
        } while (!this->policy_queue_.empty());

        if (rv) {
            while (!this->visited_.empty()) {
                const StateID sid = this->visited_.back();
                auto& info = get_lrtdp_state_info(sid);
                info.unmark();
                info.mark_solved();
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
    */
};

} // namespace lrtdp
} // namespace engines
} // namespace probfd

#endif // __LRTDP_H__
