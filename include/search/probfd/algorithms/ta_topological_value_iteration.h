#ifndef PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H
#define PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H

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

/// Namespace dedicated to trap-aware Topological Value Iteration (TATVI).
namespace probfd::algorithms::ta_topological_vi {

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
 * @brief Implements a trap-aware variant of Topological Value Iteration.
 *
 * This implementation extends topological value iteration by eliminating all
 * traps reachable from the initial state on-the-fly to guarantee the
 * convergence against the optimal value function.
 *
 * @see topological_vi::TopologicalValueIteration
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam UseInterval - Whether value intervals are used.
 */
template <typename State, typename Action, bool UseInterval = false>
class TATopologicalValueIteration : public MDPAlgorithm<State, Action> {
    using Base = typename TATopologicalValueIteration::MDPAlgorithm;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using AlgorithmValueType = algorithms::AlgorithmValueType<UseInterval>;

    struct StateInfo {
        // Status Flags
        enum { NEW, CLOSED, ONSTACK };

        uint8_t status = NEW;
        unsigned stack_id = std::numeric_limits<unsigned>::max();
        unsigned ecd_stack_id = std::numeric_limits<unsigned>::max();
    };

    struct StackInfo;

    struct ExplorationInfo {
        // Immutable state
        StateID state_id;
        StackInfo& stack_info;
        unsigned stackidx;

        // Tarjans's algorithm state
        unsigned lowlink;

        // Exploration State -- Remaining operators
        std::vector<Action> aops;

        // Exploration State -- Currently expanded transition and successor
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;

        value_t self_loop_prob = 0_vt;

        // End component decomposition state

        // recursive decomposition flag
        // Recursively decompose the SCC if there is a zero-cost transition
        // in it that can leave and remain in the scc, or a non-zero-cost
        // transition that can remain in the MDP. Both cannot be part of an
        // end component and removing them affects connectivity of the SCCs,
        // so recursion is necessary after removal.
        bool recurse : 1 = false;

        // whether the transition has non-zero cost or can leave the scc
        bool nz_or_leaves_scc : 1;

        ExplorationInfo(
            StateID state_id,
            StackInfo& stack_info,
            unsigned int stackidx);

        bool next_transition(MDP& mdp);
        bool forward_non_loop_transition(MDP& mdp, const State& state);
        bool forward_non_loop_successor();
        bool next_successor();

        ItemProbabilityPair<StateID> get_current_successor();
    };

    struct QValueInfo {
        // Precomputed part of the Q-value.
        // Sum of action cost plus those weighted successor values which
        // have already converged due to topological ordering.
        AlgorithmValueType conv_part;

        // Pointers to successor values which have not yet converged,
        // self-loops excluded.
        std::vector<ItemProbabilityPair<StateID>> scc_successors;

        explicit QValueInfo(value_t action_cost);

        bool finalize_transition(value_t self_loop_prob);

        template <typename ValueStore>
        AlgorithmValueType compute_q_value(ValueStore& value_store) const;
    };

    struct StackInfo {
        StateID state_id;

        // Reference to the state value of the state.
        AlgorithmValueType* value;

        // Precomputed portion of the Bellman update.
        // Maximum over all Q values for actions which always
        // leave the current scc.
        AlgorithmValueType conv_part;

        // Q value structs for transitions belonging to the scc,
        // but not to an end component.
        std::vector<QValueInfo> non_ec_transitions;

        // Q value structs for transitions currently assumed to belong
        // to an end component within the current scc.
        // Iteratively refined during end component decomposition.
        std::vector<QValueInfo> ec_transitions;

        StackInfo(StateID state_id, AlgorithmValueType& value_ref);

        template <typename ValueStore>
        bool update_value(ValueStore& value_store);
    };

    struct ECDExplorationInfo {
        // Immutable info
        StackInfo& stack_info;
        unsigned stackidx;

        // Exploration state - Action
        typename std::vector<QValueInfo>::iterator action;
        typename std::vector<QValueInfo>::iterator end;

        // Exploration state - Transition successor
        typename std::vector<ItemProbabilityPair<StateID>>::iterator successor;

        // Tarjan's algorithm state
        unsigned lowlink;

        // End component decomposition state

        // ECD recursion flag. Recurse if there is a transition that can leave
        // and remain in the current scc.
        bool recurse : 1 = false;

        // Whether the current transition remains in or leaves the current scc.
        bool leaves_scc : 1 = false;
        bool remains_scc : 1 = false;

        ECDExplorationInfo(StackInfo& stack_info, unsigned stackidx);

        bool next_transition();
        bool next_successor();

        ItemProbabilityPair<StateID> get_current_successor();
    };

    storage::PerStateStorage<StateInfo> state_information_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::deque<StackInfo> stack_;

    std::deque<ECDExplorationInfo> exploration_stack_ecd_;
    std::deque<StateID> stack_ecd_;

    Statistics statistics_;

public:
    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ProgressReport,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

    /**
     * @brief Retreive the algorithm statistics.
     */
    [[nodiscard]]
    Statistics get_statistics() const;

    /**
     * @brief Runs the algorithm with the supplied state value storage.
     *
     * Computes the full optimal value function for the entire state space
     * reachable from \p initial_state. Stores the state values in the
     * output parameter \p value_store. Returns the value of the initial state.
     */
    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        StateID init_state_id,
        auto& value_store,
        double max_time = std::numeric_limits<double>::infinity());

private:
    /**
     * Pushes a state onto the exploration queue and stack.
     */
    void push_state(
        StateID state_id,
        StateInfo& state_info,
        AlgorithmValueType& value);

    /**
     * Initializes the data structures for this new state and pushes it
     * onto the queue unless it is terminal modulo self-loops. Returns
     * true if the state was pushed.
     */
    bool initialize_state(
        MDP& mdp,
        Evaluator& heuristic,
        ExplorationInfo& exp_info,
        auto& value_store);

    /**
     * Iterates over all possible successors and tries to find a new
     * non-terminal state. If such a state is found, pushes it and
     * return true, otherwise returns false.
     *
     * Initializes the data structures of all newly encountered successor
     * states, and updates the data structures of the source state.
     */
    bool successor_loop(
        MDP& mdp,
        ExplorationInfo& explore,
        auto& value_store,
        utils::CountdownTimer& timer);

    /**
     * Handle the new SCC and perform value iteration on it.
     */
    void scc_found(
        auto& value_store,
        ExplorationInfo& exp_info,
        auto scc,
        utils::CountdownTimer& timer);

    void find_and_decompose_sccs(
        StateID state_id,
        StateInfo& state_info,
        utils::CountdownTimer& timer);

    bool initialize_ecd(ECDExplorationInfo& exp_info);

    bool
    push_successor_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer);

    void scc_found_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer);
};

} // namespace probfd::algorithms::ta_topological_vi

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H
#include "probfd/algorithms/ta_topological_value_iteration_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H

#endif // PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H