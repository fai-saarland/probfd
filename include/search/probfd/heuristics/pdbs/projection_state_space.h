#ifndef PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H
#define PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H

#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/projection_operator.h"
#include "probfd/heuristics/pdbs/state_rank.h"


#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_space.h"

#include <limits>
#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {

class StateRankingFunction;

/// Represents the state space of a projection of a probabilistic planning task.
class ProjectionStateSpace
    : public engine_interfaces::
          StateSpace<StateRank, const ProjectionOperator*> {

    MatchTree match_tree_;

public:
    ProjectionStateSpace(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& ranking_function,
        TaskCostFunction& task_cost_function,
        bool operator_pruning = true,
        double max_time = std::numeric_limits<double>::infinity());

    StateID get_state_id(StateRank state) override;

    StateRank get_state(StateID id) override;

    ActionID get_action_id(StateID, const ProjectionOperator* op) override;

    const ProjectionOperator* get_action(StateID, ActionID action_id) override;

    void generate_applicable_actions(
        StateID state,
        std::vector<const ProjectionOperator*>& aops) override;

    void generate_action_transitions(
        StateID state,
        const ProjectionOperator* op,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        StateID state,
        std::vector<const ProjectionOperator*>& aops,
        std::vector<Distribution<StateID>>& result) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif