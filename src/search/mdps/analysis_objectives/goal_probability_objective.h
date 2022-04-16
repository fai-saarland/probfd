#ifndef MDPS_ANALYSIS_OBJECTIVES_GOAL_PROBABILITY_OBJECTIVE_H
#define MDPS_ANALYSIS_OBJECTIVES_GOAL_PROBABILITY_OBJECTIVE_H

#include "analysis_objective.h"

#include <memory>

namespace probabilistic {

class ProbabilisticOperator;

namespace analysis_objectives {

/**
 * @brief The MaxProb analysis objective.
 *
 * Implements the MaxProb anaylsis objective. State reward is one
 * for goal states and zero otherwise. Actions have no reward.
 *
 */
class GoalProbabilityObjective : public AnalysisObjective {
public:
    /// Default constructor.
    explicit GoalProbabilityObjective();

    virtual value_utils::IntervalValue reward_bound() override;
    virtual GlobalStateRewardFunction* state_reward() override;
    virtual ProbabilisticOperatorRewardFunction* action_reward() override;

private:
    std::unique_ptr<GlobalStateRewardFunction> state_eval_;
    std::unique_ptr<ProbabilisticOperatorRewardFunction> action_eval_;
};

} // namespace analysis_objectives
} // namespace probabilistic

#endif // __GOAL_PROBABILITY_OBJECTIVE_H__