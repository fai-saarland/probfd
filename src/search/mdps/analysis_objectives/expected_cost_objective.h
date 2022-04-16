#ifndef MDPS_ANALYSIS_OBJECTIVES_EXPECTED_COST_OBJECTIVE_H
#define MDPS_ANALYSIS_OBJECTIVES_EXPECTED_COST_OBJECTIVE_H

#include "analysis_objective.h"

#include <memory>

namespace probabilistic {

class ProbabilisticOperator;

namespace analysis_objectives {

/**
 * @brief The expected-cost anaylsis objective.
 *
 * Implements the expected-cost analysis objective. State rewards
 * are zero. Action rewards are their negative costs.
 *
 */
class ExpectedCostObjective : public AnalysisObjective {
public:
    /// Default constructor.
    explicit ExpectedCostObjective();

    virtual value_utils::IntervalValue reward_bound() override;
    virtual GlobalStateRewardFunction* state_reward() override;
    virtual ProbabilisticOperatorRewardFunction* action_reward() override;

private:
    std::unique_ptr<GlobalStateRewardFunction> state_eval_;
    std::unique_ptr<ProbabilisticOperatorRewardFunction> action_eval_;
};

} // namespace analysis_objectives
} // namespace probabilistic

#endif // __EXPECTED_COST_OBJECTIVE_H__