#ifndef MDPS_ANALYSIS_OBJECTIVES_GOAL_PROBABILITY_OBJECTIVE_H
#define MDPS_ANALYSIS_OBJECTIVES_GOAL_PROBABILITY_OBJECTIVE_H

#include "probfd/analysis_objectives/analysis_objective.h"

#include <memory>

namespace probfd {

class ProbabilisticOperator;

namespace analysis_objectives {

/**
 * @brief The MaxProb analysis objective.
 *
 * Implements the MaxProb analysis objective. State reward is one
 * for goal states and zero otherwise. Actions have no reward.
 *
 */
class GoalProbabilityObjective : public AnalysisObjective {
public:
    /// Default constructor.
    explicit GoalProbabilityObjective();

    virtual value_utils::IntervalValue reward_bound() override;
    virtual TaskRewardFunction* reward() override;

private:
    std::unique_ptr<TaskRewardFunction> reward_;
};

} // namespace analysis_objectives
} // namespace probfd

#endif // __GOAL_PROBABILITY_OBJECTIVE_H__