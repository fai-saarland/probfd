#ifndef PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H
#define PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/engine_interfaces.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {

class StateRankingFunction;

/// Represents the state space of a projection of a probabilistic planning task.
class ProjectionStateSpace {
    // For now, only the PDB implementations need this class.
    friend ProbabilisticPatternDatabase;
    friend MaxProbPatternDatabase;
    friend SSPPatternDatabase;

    engine_interfaces::StateSpace<StateRank, const AbstractOperator*>
        state_space;

    StateRank initial_state_;

    std::vector<bool> goal_state_flags_;

public:
    ProjectionStateSpace(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& ranking_function,
        bool operator_pruning = true);

    StateID get_state_id(heuristics::pdbs::StateRank state) const;

    StateRank get_state(StateID id) const;

    ActionID get_action_id(StateID, const AbstractOperator* op) const;

    const AbstractOperator* get_action(StateID, ActionID action_id) const;

    void generate_applicable_actions(
        StateID state,
        std::vector<const AbstractOperator*>& aops);

    void generate_action_transitions(
        StateID state,
        const AbstractOperator* op,
        Distribution<StateID>& result);

    void generate_all_transitions(
        StateID state,
        std::vector<const AbstractOperator*>& aops,
        std::vector<Distribution<StateID>>& result);

    bool is_goal(StateRank rank) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif