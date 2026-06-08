#ifndef PROBFD_PDBS_PROJECTION_STATE_SPACE_H
#define PROBFD_PDBS_PROJECTION_STATE_SPACE_H

#include "downward/utils/timer.h"
#include "probfd/pdbs/match_tree.h"
#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/mdp.h"
#include "probfd/probabilistic_task.h"

#include <limits>
#include <vector>

namespace probfd {
template <typename>
class Distribution;
class TerminationCosts;
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
    std::shared_ptr<downward::OperatorCostFunction<value_t>>
        parent_cost_function_;
    std::shared_ptr<TerminationCosts> parent_term_function_;
    std::vector<bool> goal_state_flags_;

public:
    ProjectionStateSpace(
        SharedProbabilisticTask task,
        const StateRankingFunction& ranking_function,
        bool operator_pruning = true,
        downward::utils::FSeconds max_time = downward::utils::FSeconds::max());

    StateID get_state_id(StateRank state) final;

    StateRank get_state(StateID id) final;

    void generate_applicable_actions(
        StateRank state,
        std::vector<const ProjectionOperator*>& aops) final;

    void generate_action_transitions(
        StateRank state,
        const ProjectionOperator* op,
        SuccessorDistribution& successor_dist) final;

    void generate_all_transitions(
        StateRank state,
        std::vector<const ProjectionOperator*>& aops,
        std::vector<SuccessorDistribution>& successor_dist) final;

    void generate_all_transitions(
        StateRank state,
        std::vector<TransitionTailType>& transitions) final;

    StateID get_state_id(StateRank state) const;

    StateRank get_state(StateID id) const;

    void generate_applicable_actions(
        StateRank state,
        std::vector<const ProjectionOperator*>& aops) const;

    void generate_action_transitions(
        StateRank state,
        const ProjectionOperator* op,
        SuccessorDistribution& successor_dist) const;

    void generate_all_transitions(
        StateRank state,
        std::vector<const ProjectionOperator*>& aops,
        std::vector<SuccessorDistribution>& successor_dist) const;

    void generate_all_transitions(
        StateRank state,
        std::vector<TransitionTailType>& transitions) const;

    [[nodiscard]]
    bool is_goal(StateRank state) const override;

    [[nodiscard]]
    value_t get_goal_termination_cost() const override;

    [[nodiscard]]
    value_t get_non_goal_termination_cost() const override;

    value_t get_action_cost(const ProjectionOperator* op) override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROJECTION_STATE_SPACE_H
