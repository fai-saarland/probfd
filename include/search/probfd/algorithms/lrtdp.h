#ifndef PROBFD_ALGORITHMS_LRTDP_H
#define PROBFD_ALGORITHMS_LRTDP_H

#include "probfd/algorithms/heuristic_search_base.h"

#include <deque>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace algorithms {

template <typename>
class SuccessorSampler;

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

    void print(std::ostream& out) const;
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
    heuristic_search::
        HeuristicSearchAlgorithm<State, Action, UseInterval, true>,
    heuristic_search::HeuristicSearchAlgorithm<
        State,
        Action,
        UseInterval,
        true,
        internal::PerStateInformation>>;

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
 * @tparam Fret - Specifies whether the algorithm should be usable within FRET.
 */
template <typename State, typename Action, bool UseInterval, bool Fret>
class LRTDP : public internal::LRTDPBase<State, Action, UseInterval, Fret> {
    using Base = typename LRTDP::HeuristicSearchAlgorithm;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;
    using PolicyPicker = typename Base::PolicyPicker;

    using SuccessorSampler = SuccessorSampler<Action>;

    using StateInfo = std::conditional_t<
        Fret,
        internal::PerStateInformation<internal::EmptyStateInfo>,
        typename Base::StateInfo>;

    using Statistics = internal::Statistics;

    const TrialTerminationCondition StopConsistent;
    std::shared_ptr<SuccessorSampler> sample_;

    storage::PerStateStorage<StateInfo> state_infos_;

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
        ProgressReport* report,
        bool interval_comparison,
        TrialTerminationCondition stop_consistent,
        std::shared_ptr<SuccessorSampler> succ_sampler);

    void reset_search_state() override;

protected:
    Interval do_solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time) override;

    void print_additional_statistics(std::ostream& out) const override;

    void setup_custom_reports(param_type<State>) override;

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

    StateInfo& get_lrtdp_state_info(StateID sid);

    /*
    bool check_and_solve_original(
        MDP& mdp,
        Evaluator& heuristic,
        StateID init_state_id);
    */
};

} // namespace lrtdp
} // namespace algorithms
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H
#include "probfd/algorithms/lrtdp_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_LRTDP_H

#endif // __LRTDP_H__
