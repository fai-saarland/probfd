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

    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;

    using AlgorithmValueType = algorithms::AlgorithmValue<UseInterval>;

    struct StateInfo {
        // Status Flags
        enum { NEW, CLOSED, ONSTACK };

        static constexpr uint32_t UNDEF =
            std::numeric_limits<uint32_t>::max() >> 1;
        static constexpr uint32_t UNDEF_ECD =
            std::numeric_limits<uint32_t>::max();

        unsigned explored : 1 = 0;
        unsigned stack_id : 31 = UNDEF;
        unsigned ecd_stack_id = UNDEF_ECD;

        [[nodiscard]]
        auto get_status() const;

        [[nodiscard]]
        auto get_ecd_status() const;
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

        Interval exit_interval;
        value_t self_loop_prob = 0_vt;

        // End component decomposition state

        // recursive decomposition flag
        // Recursively decompose the SCC if there is a zero-cost transition
        // in it that can leave and remain in the scc, or a non-zero-cost
        // transition that can remain in the MDP. Both cannot be part of an
        // end component and removing them affects connectivity of the SCCs,
        // so recursion is necessary after removal.
        bool recurse : 1 = false;
        bool has_all_zero : 1 = true;

        // whether the transition has non-zero cost or can leave the scc
        bool non_zero : 1;
        bool leaves_scc : 1 = false;

        ExplorationInfo(
            StateID state_id,
            StackInfo& stack_info,
            unsigned int stackidx);

        bool next_transition(MDPType& mdp);
        bool forward_non_loop_transition(MDPType& mdp, const State& state);
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

        struct ParentTransition {
            unsigned parent_idx;
            unsigned parent_transition_idx;
        };

        struct TransitionFlags {
            bool is_active_exiting : 1; // Is the transition active and an SCC
                                        // exit?
            bool is_active : 1;         // Is the transition active?
        };

        unsigned active_exit_transitions =
            0;                           // Number of active exit transitions.
        unsigned active_transitions = 0; // Number of active transitions.

        std::vector<TransitionFlags> transition_flags;
        std::vector<ParentTransition> parents;

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

    struct DecompositionQueue {
        std::vector<StateID> state_ids;
        std::vector<std::size_t> scc_spans;

        void reserve(std::size_t num_states)
        {
            state_ids.reserve(num_states);
            scc_spans.reserve(num_states);
        }

        void register_new_scc() { scc_spans.push_back(state_ids.size()); }

        void add_scc_state(StateID state_id) { state_ids.push_back(state_id); }

        bool pop_scc(std::vector<StateID>& r)
        {
            using namespace std::views;

            assert(r.empty());

            if (state_ids.empty()) return false;

            auto scc_view = state_ids | drop(scc_spans.back());

            for (const auto state_id : scc_view) {
                r.push_back(state_id);
            }

            state_ids.erase(scc_view.begin(), scc_view.end());

            scc_spans.pop_back();

            return true;
        }
    };

    storage::PerStateStorage<StateInfo> state_information_;
    std::vector<ExplorationInfo> exploration_stack_;
    std::deque<StackInfo> stack_;

    std::vector<ECDExplorationInfo> exploration_stack_ecd_;
    std::vector<StateID> stack_ecd_;

    DecompositionQueue decomposition_queue_;
    std::vector<StateID> scc_;

    Statistics statistics_;

public:
    TATopologicalValueIteration() = default;

    explicit TATopologicalValueIteration(std::size_t num_states_hint)
    {
        exploration_stack_.reserve(num_states_hint);
        exploration_stack_ecd_.reserve(num_states_hint);
        stack_ecd_.reserve(num_states_hint);
        decomposition_queue_.reserve(num_states_hint);
        scc_.reserve(num_states_hint);
    }

    Interval solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
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
        MDPType& mdp,
        const EvaluatorType& heuristic,
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
        MDPType& mdp,
        const EvaluatorType& heuristic,
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
        MDPType& mdp,
        ExplorationInfo& explore,
        auto& value_store,
        utils::CountdownTimer& timer);

    /**
     * Handle the new SCC and perform value iteration on it.
     */
    void scc_found(
        auto& value_store,
        ExplorationInfo& exp_info,
        unsigned int stack_idx,
        utils::CountdownTimer& timer);

    void find_and_decompose_sccs(utils::CountdownTimer& timer);

    bool initialize_ecd(ECDExplorationInfo& exp_info);

    bool
    push_successor_ecd(ECDExplorationInfo& e, utils::CountdownTimer& timer);

    void scc_found_ecd(ECDExplorationInfo& e);
};

} // namespace probfd::algorithms::ta_topological_vi

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H
#include "probfd/algorithms/ta_topological_value_iteration_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H

#endif // PROBFD_ALGORITHMS_TA_TOPOLOGICAL_VALUE_ITERATION_H