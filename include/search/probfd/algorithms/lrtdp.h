#ifndef PROBFD_ALGORITHMS_LRTDP_H
#define PROBFD_ALGORITHMS_LRTDP_H

#include "probfd/algorithms/heuristic_search_base.h"

#include <deque>

// Forward Declarations
namespace utils {
class CountdownTimer;
}

namespace probfd::algorithms {
template <typename>
class SuccessorSampler;
}

/// Namespace dedicated to labelled real-time dynamic programming (LRTDP).
namespace probfd::algorithms::lrtdp {

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

    void print(std::ostream& out) const;
};

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr uint8_t MARKED_OPEN = (1 << StateInfo::BITS);
    static constexpr uint8_t MARKED_TRIAL = (2 << StateInfo::BITS);
    static constexpr uint8_t SOLVED = (4 << StateInfo::BITS);
    static constexpr uint8_t BITS = StateInfo::BITS + 3;
    static constexpr uint8_t MASK = (7 << StateInfo::BITS);

    [[nodiscard]]
    bool is_marked_open() const
    {
        return this->info & MARKED_OPEN;
    }

    [[nodiscard]]
    bool is_marked_trial() const
    {
        return this->info & MARKED_TRIAL;
    }

    [[nodiscard]]
    bool is_solved() const
    {
        return this->info & SOLVED;
    }

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

    void clear() { this->info &= ~MASK; }
};

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
 * \ref algorithms::SuccessorSampler describes how successor
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
 */
template <typename State, typename Action, bool UseInterval>
class LRTDP
    : public heuristic_search::HeuristicSearchAlgorithmExt<
          State,
          Action,
          UseInterval,
          true,
          internal::PerStateInformation> {
    using Base = typename LRTDP::HeuristicSearchAlgorithm;

public:
    using StateInfo = typename Base::StateInfo;

private:
    using MDP = typename Base::MDPType;
    using Evaluator = typename Base::EvaluatorType;
    using PolicyPicker = typename Base::PolicyPicker;

    using SuccessorSampler = SuccessorSampler<Action>;

    using Statistics = internal::Statistics;

    const TrialTerminationCondition stop_consistent_;
    std::shared_ptr<SuccessorSampler> sample_;

    std::vector<StateID> current_trial_;
    std::vector<StateID> policy_queue_;
    std::deque<StateID> visited_;

    Statistics statistics_;

public:
    /**
     * @brief Constructs an LRTDP solver object.
     */
    LRTDP(
        std::shared_ptr<PolicyPicker> policy_chooser,
        TrialTerminationCondition stop_consistent,
        std::shared_ptr<SuccessorSampler> succ_sampler);

    void reset_search_state() override;

protected:
    Interval do_solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ProgressReport& progress,
        double max_time) override;

    void print_additional_statistics(std::ostream& out) const override;

    void
    setup_custom_reports(param_type<State>, ProgressReport& progress) override;

private:
    void trial(
        MDP& mdp,
        Evaluator& heuristic,
        StateID initial_state,
        utils::CountdownTimer& timer);

    bool check_and_solve(
        MDP& mdp,
        Evaluator& heuristic,
        StateID init_state_id,
        utils::CountdownTimer& timer);

    /*
    bool check_and_solve_original(
        MDP& mdp,
        Evaluator& heuristic,
        StateID init_state_id);
    */
};

} // namespace probfd::algorithms::lrtdp

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H
#include "probfd/algorithms/lrtdp_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H

#endif // PROBFD_ALGORITHMS_LRTDP_H
