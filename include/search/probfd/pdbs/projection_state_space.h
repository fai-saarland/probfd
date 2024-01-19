#ifndef PROBFD_PDBS_PROJECTION_STATE_SPACE_H
#define PROBFD_PDBS_PROJECTION_STATE_SPACE_H

#include "probfd/pdbs/match_tree.h"
#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/mdp.h"

#include <limits>
#include <vector>

namespace probfd {
template <typename>
class Distribution;
class ProbabilisticTaskProxy;
} // namespace probfd

namespace probfd::pdbs {
class ProjectionOperator;
class StateRankingFunction;
} // namespace probfd::pdbs

namespace probfd::pdbs {

/// Represents the state space of a projection of a probabilistic planning
/// task.
class ProjectionStateSpace
    : public SimpleMDP<StateRank, const ProjectionOperator*> {

    MatchTree match_tree_;
    FDRSimpleCostFunction* parent_cost_function_;
    std::vector<bool> goal_state_flags_;

public:
    ProjectionStateSpace(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const StateRankingFunction& ranking_function,
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

    [[nodiscard]]
    bool is_goal(StateRank state) const override;

    [[nodiscard]]
    value_t get_non_goal_termination_cost() const override;

    value_t get_action_cost(const ProjectionOperator* op) override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROJECTION_STATE_SPACE_H
