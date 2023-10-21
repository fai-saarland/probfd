#ifndef PROBFD_ALGORITHMS_TOPOLOGICAL_VALUE_ITERATION_H
#define PROBFD_ALGORITHMS_TOPOLOGICAL_VALUE_ITERATION_H

#include "probfd/algorithms/types.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"
#include "probfd/mdp_algorithm.h"

#include <deque>
#include <limits>
#include <ostream>
#include <vector>

namespace utils {
class CountdownTimer;
}

namespace probfd {

namespace policies {
template <typename, typename>
class MapPolicy;
}

namespace algorithms {

/// Namespace dedicated to Topological Value Iteration (TVI).
namespace topological_vi {

/**
 * @brief Topological value iteration statistics.
 */
struct Statistics {
    unsigned long long expanded_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long sccs = 0;
    unsigned long long singleton_sccs = 0;
    unsigned long long bellman_backups = 0;
    unsigned long long pruned = 0;

    void print(std::ostream& out) const;
};

/**
 * @brief Implements Topological Value Iteration \cite dai:etal:jair-11.
 *
 * This algorithm performs value iteration on each strongly connected component
 * of the MDP in reverse topological order. This implementation exhaustively
 * explores the entire reachable state space and computes a full optimal state
 * value table by default. A heuristic can be supplied to explicitly prune
 * parts of the state space or to accelerate convergence.
 *
 * This implementation also supports value intervals. However, convergence
 * is not guaranteed with value intervals if traps exist within the reachable
 * state space. In this case, traps must be removed prior to running topological
 * value iteration, or the trap-aware variant TATopologicalValueIteration of
 * this algorithm must be used, which eliminates as traps on-the-fly to
 * guarantee convergence.
 *
 * @see interval_iteration::IntervalIteration
 * @see ta_topological_value_iteration::TATopologicalValueIteration
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam UseInterval - Whether value intervals are used.
 */
template <typename State, typename Action, bool UseInterval = false>
class TopologicalValueIteration : public MDPAlgorithm<State, Action> {
    using Base = typename TopologicalValueIteration::MDPAlgorithm;

    using Policy = typename Base::Policy;
    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using MapPolicy = policies::MapPolicy<State, Action>;
    using AlgorithmValueType = algorithms::AlgorithmValueType<UseInterval>;

    struct StateInfo {
        // Status Flags
        enum { NEW, CLOSED, ONSTACK };

        unsigned stack_id = 0;
        uint8_t status = NEW;
    };

    struct ExplorationInfo {
        // Exploration State
        std::vector<Action> aops;         // Remaining unexpanded operators
        Distribution<StateID> transition; // Currently expanded transition
        Distribution<StateID>::const_iterator successor; // Current successor

    public:
        // Immutable info
        StateID state_id;  // State this information belongs to
        unsigned stackidx; // Index on the stack of the associated state

        unsigned lowlink;

        ExplorationInfo(
            std::vector<Action> aops,
            Distribution<StateID> transition,
            StateID state_id,
            unsigned stackidx);

        void update_lowlink(unsigned upd);

        bool next_transition(MDP& mdp, StateID state_id);
        bool next_successor();

        Action& get_current_action();
        ItemProbabilityPair<StateID> get_current_successor();
    };

    struct QValueInfo {
        // The action id this Q value belongs to.
        Action action;

        // Probability to remain in the same state.
        // Casted to the self-loop normalization factor after
        // finalize_transition().
        value_t self_loop_prob = 0_vt;

        // Precomputed part of the Q-value.
        // Sum of action cost plus those weighted successor values which
        // have already converged due to topological ordering.
        AlgorithmValueType conv_part;

        // Pointers to successor values which have not yet converged,
        // self-loops excluded.
        std::vector<ItemProbabilityPair<AlgorithmValueType*>> nconv_successors;

        QValueInfo(Action action, value_t action_cost);

        bool finalize_transition();

        AlgorithmValueType compute_q_value() const;
    };

    struct StackInfo {
        StateID state_id;

        // Reference to the state value of the state.
        AlgorithmValueType* value;

        // The state cost
        AlgorithmValueType terminal_cost;

        // Precomputed part of the max of the value update.
        // Minimum over all Q values of actions leaving the SCC.
        AlgorithmValueType conv_part;

        // Remaining Q values which have not yet converged.
        std::vector<QValueInfo> nconv_qs;

        // The optimal action
        std::optional<Action> best_action = std::nullopt;

        // The optimal action among those leaving the SCC.
        std::optional<Action> best_converged = std::nullopt;

        StackInfo(
            StateID state_id,
            AlgorithmValueType& value_ref,
            value_t terminal_cost,
            unsigned num_aops);

        bool update_value();
    };

    using StackIterator = typename std::vector<StackInfo>::iterator;

    const bool expand_goals_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    Statistics statistics_;

public:
    explicit TopologicalValueIteration(bool expand_goals);

    /**
     * \copydoc MDPAlgorithm::compute_policy(param_type<State>, double)
     */
    std::unique_ptr<Policy> compute_policy(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ProgressReport,
        double max_time) override;

    /**
     * \copydoc MDPAlgorithm::solve(param_type<State>, double)
     */
    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ProgressReport,
        double max_time) override;

    /**
     * \copydoc MDPAlgorithm::print_statistics(std::ostream&) const
     */
    void print_statistics(std::ostream& out) const override;

    /**
     * @brief Retreive the algorithm statistics.
     */
    Statistics get_statistics() const;

    /**
     * @brief Runs the algorithm with the supplied state value storage.
     *
     * Computes the full optimal value function for the entire state space
     * reachable from \p initial_state. Stores the state values in the
     * output parameter \p value_store. Returns the value of the initial state.
     */
    template <typename ValueStore>
    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        StateID init_state_id,
        ValueStore& value_store,
        double max_time = std::numeric_limits<double>::infinity(),
        MapPolicy* policy = nullptr);

private:
    /**
     * Initializes the data structures for this new state and pushes it
     * onto the queue unless it is terminal modulo self-loops. Returns
     * true if the state was pushed.
     */
    bool push_state(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state_id,
        StateInfo& state_info,
        AlgorithmValueType& state_value);

    /**
     * Iterates over all possible successors and tries to find a new
     * non-terminal state. If such a state is found, pushes it and
     * return true, otherwise returns false.
     *
     * Initializes the data structures of all newly encountered successor
     * states, and updates the data structures of the source state.
     */
    template <typename ValueStore>
    bool successor_loop(
        MDP& mdp,
        Evaluator& heuristic,
        ExplorationInfo& explore,
        StackInfo& stack_info,
        StateID state_id,
        ValueStore& value_store,
        utils::CountdownTimer& timer);

    /**
     * Handle the new SCC and perform value iteration on it.
     */
    void scc_found(
        StackIterator begin,
        StackIterator end,
        MapPolicy* policy,
        utils::CountdownTimer& timer);
};

} // namespace topological_vi
} // namespace algorithms
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TOPOLOGICAL_VALUE_ITERATION_H
#include "probfd/algorithms/topological_value_iteration_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TOPOLOGICAL_VALUE_ITERATION_H

#endif // __TOPOLOGICAL_VALUE_ITERATION_H__