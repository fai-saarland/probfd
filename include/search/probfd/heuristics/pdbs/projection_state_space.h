#ifndef PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H
#define PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_space.h"

#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {

class StateRankingFunction;

/// Represents the state space of a projection of a probabilistic planning task.
class ProjectionStateSpace
    : public engine_interfaces::StateSpace<StateRank, const AbstractOperator*> {

    MatchTree match_tree_;

public:
    ProjectionStateSpace(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& ranking_function,
        TaskCostFunction& task_cost_function,
        bool operator_pruning = true);

    StateID get_state_id(StateRank state) override;

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
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif