#ifndef PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H
#define PROBFD_HEURISTICS_PDBS_PROJECTION_STATE_SPACE_H

#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/projection_operator.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/fdr_types.h"
#include "probfd/mdp.h"

#include <limits>
#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {

class ProjectionOperator;
class StateRankingFunction;

/// Represents the state space of a projection of a probabilistic planning task.
class ProjectionStateSpace
    : public SimpleMDP<StateRank, const ProjectionOperator*> {

    MatchTree match_tree_;
    FDRSimpleCostFunction* parent_cost_function;
    std::vector<bool> goal_state_flags_;

public:
    ProjectionStateSpace(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& ranking_function,
        FDRSimpleCostFunction& task_cost_function,
        bool operator_pruning = true,
        double max_time = std::numeric_limits<double>::infinity());

    StateID get_state_id(StateRank state) override;

    StateRank get_state(StateID id) override;

    void generate_applicable_actions(
        StateRank state,
        std::vector<const ProjectionOperator*>& aops) override;

    void generate_action_transitions(
        StateRank state,
        const ProjectionOperator* op,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        StateRank state,
        std::vector<const ProjectionOperator*>& aops,
        std::vector<Distribution<StateID>>& result) override;

    void generate_all_transitions(
        StateRank state,
        std::vector<Transition>& transitions) override;

    bool is_goal(StateRank state) const override;

    value_t get_non_goal_termination_cost() const override;

    value_t get_action_cost(const ProjectionOperator* op) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif