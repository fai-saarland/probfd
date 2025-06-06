#ifndef PROBFD_ALGORITHMS_LRTDP_H
#define PROBFD_ALGORITHMS_LRTDP_H

#include "probfd/algorithms/heuristic_search_base.h"

#include <deque>

// Forward Declarations
namespace downward::utils {
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

template <typename Action, bool UseInterval>
struct PerStateInformation
    : public heuristic_search::
          PerStateBaseInformation<Action, true, UseInterval> {
private:
    using Base = typename heuristic_search::PerStateBaseInformation<Action, true, UseInterval>;

public:
    static constexpr uint8_t VISITED = 0b01 << Base::BITS;
    static constexpr uint8_t SOLVED = 0b10 << Base::BITS;
    static constexpr uint8_t BITS = Base::BITS + 2;
    static constexpr uint8_t MASK = 0b11 << Base::BITS;

    bool is_solved() const
    {
        return this->info & SOLVED || this->is_goal_or_terminal();
    }

    void mark_solved() { this->info |= SOLVED; }

    bool is_on_trial() const { return (this->info & VISITED) != 0; }

    void set_on_trial()
    {
        assert(!is_solved());
        this->info |= VISITED;
    }

    void clear_trial_flag()
    {
        assert(!is_solved());
        this->info &= ~VISITED;
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
    : public heuristic_search::HeuristicSearchAlgorithm<
          State,
          Action,
          internal::PerStateInformation<Action, UseInterval>> {
    using Base = typename LRTDP::HeuristicSearchAlgorithm;

    using AlgorithmValueType = typename Base::AlgorithmValueType;

public:
    using StateInfo = typename Base::StateInfo;

private:
    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;
    using PolicyPickerType = typename Base::PolicyPicker;

    using SuccessorSamplerType = SuccessorSampler<Action>;

    using Statistics = internal::Statistics;

    // Algorithm parameters
    const TrialTerminationCondition stop_consistent_;
    const std::shared_ptr<SuccessorSamplerType> sample_;

    // Algorithm state
    std::vector<StateID> current_trial_;
    std::vector<StateID> policy_queue_;
    std::deque<StateID> visited_;

    Statistics statistics_;

    // Re-used buffer
    std::vector<TransitionTail<Action>> transitions_;
    std::vector<AlgorithmValueType> qvalues_;

public:
    /**
     * @brief Constructs an LRTDP solver object.
     */
    LRTDP(
        value_t epsilon,
        std::shared_ptr<PolicyPickerType> policy_chooser,
        TrialTerminationCondition stop_consistent,
        std::shared_ptr<SuccessorSamplerType> succ_sampler);

protected:
    Interval do_solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport& progress,
        double max_time) override;

    void print_additional_statistics(std::ostream& out) const override;

private:
    bool trial(
        MDPType& mdp,
        HeuristicType& heuristic,
        StateID initial_state,
        downward::utils::CountdownTimer& timer);

    bool check_and_solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        StateID init_state_id,
        downward::utils::CountdownTimer& timer);
};

} // namespace probfd::algorithms::lrtdp

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H
#include "probfd/algorithms/lrtdp_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H

#endif // PROBFD_ALGORITHMS_LRTDP_H
