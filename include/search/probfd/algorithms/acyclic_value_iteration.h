#ifndef PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H
#define PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"
#include "probfd/mdp_algorithm.h"

#include <stack>

// Forward Declarations
namespace utils {
class CountdownTimer;
}

namespace probfd::policies {
template <typename, typename>
class MapPolicy;
}

/// Namespace dedicated to the acyclic value iteration algorithm.
namespace probfd::algorithms::acyclic_vi {

namespace internal {

template <typename Action>
struct StateInfo {
    bool expanded = false;
    std::optional<Action> best_action = std::nullopt;
    value_t value;
};

struct Statistics {
    unsigned long long state_expansions = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long pruned = 0;

    void print(std::ostream& out) const
    {
        out << "  Expanded state(s): " << state_expansions << std::endl;
        out << "  Pruned state(s): " << pruned << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
    }
};
} // namespace internal

/**
 * @brief Implements acyclic Value Iteration.
 *
 * Performs value iteration on acyclic MDPs by performing a depth-first search
 * ending at terminal states and applying Bellman updates during backtracking.
 * Exactly one Bellman update is performed per encountered state.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @remark The search algorithm does not validate that the state space is
 * acyclic. It is an error to invoke this search algorithm on state spaces which
 * contain cycles.
 */
template <typename State, typename Action>
class AcyclicValueIteration : public MDPAlgorithm<State, Action> {
    using Base = typename AcyclicValueIteration::MDPAlgorithm;

    using Policy = typename Base::Policy;
    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using MapPolicy = policies::MapPolicy<State, Action>;

    using StateInfo = internal::StateInfo<Action>;
    using Statistics = internal::Statistics;

    struct IncrementalExpansionInfo {
        const StateID state_id;
        StateInfo& state_info;

        // Applicable operators left to expand
        std::vector<Action> remaining_aops;

        // The current transition and transition successor
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;

        // The current transition Q-value
        value_t t_value;

        IncrementalExpansionInfo(
            StateID state_id,
            StateInfo& state_info,
            std::vector<Action> remaining_aops,
            MDP& mdp);

        bool next_successor();
        bool next_transition(MDP& mdp, MapPolicy* policy);

        void backtrack_successor(value_t probability, StateInfo& succ_info);

    private:
        void setup_transition(MDP& mdp);
        void finalize_transition();
        void finalize_expansion(MapPolicy* policy);
    };

    Statistics statistics_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::stack<IncrementalExpansionInfo> expansion_stack_;

public:
    std::unique_ptr<Policy> compute_policy(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> initial_state,
        ProgressReport progress,
        double max_time) override;

    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> initial_state,
        ProgressReport progress,
        double max_time) override;

    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> initial_state,
        double max_time,
        MapPolicy* policy);

    void print_statistics(std::ostream& out) const override;

private:
    void dfs_expand(
        MDP& mdp,
        Evaluator& heuristic,
        utils::CountdownTimer& timer,
        MapPolicy* policy);

    bool
    dfs_backtrack(MDP& mdp, utils::CountdownTimer& timer, MapPolicy* policy);

    bool push_state(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state_id,
        StateInfo& succ_info);
};

} // namespace probfd::algorithms::acyclic_vi

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H

#include "probfd/algorithms/acyclic_value_iteration_impl.h"

#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H

#endif // PROBFD_ALGORITHMS_ACYCLIC_VALUE_ITERATION_H