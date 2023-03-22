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
class ProjectionStateSpace
    : public engine_interfaces::StateSpace<
          heuristics::pdbs::StateRank,
          const heuristics::pdbs::AbstractOperator*> {
    // For now, only the PDB implementations need this class.
    friend ProbabilisticPatternDatabase;
    friend MaxProbPatternDatabase;
    friend SSPPatternDatabase;

    heuristics::pdbs::MatchTree match_tree_;

    StateRank initial_state_;

    std::vector<bool> goal_state_flags_;

public:
    ProjectionStateSpace(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& ranking_function,
        bool operator_pruning = true);

    StateID get_state_id(heuristics::pdbs::StateRank state) override;

    StateRank get_state(StateID id) override;

    ActionID get_action_id(StateID, const AbstractOperator* op) override;

    const AbstractOperator* get_action(StateID, ActionID action_id) override;

    void generate_applicable_actions(
        StateID state,
        std::vector<const AbstractOperator*>& aops) override;

    void generate_action_transitions(
        StateID state,
        const AbstractOperator* op,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        StateID state,
        std::vector<const AbstractOperator*>& aops,
        std::vector<Distribution<StateID>>& result) override;

    bool is_goal(StateRank rank) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif