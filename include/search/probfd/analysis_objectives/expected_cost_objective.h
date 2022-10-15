#ifndef MDPS_ANALYSIS_OBJECTIVES_EXPECTED_COST_OBJECTIVE_H
#define MDPS_ANALYSIS_OBJECTIVES_EXPECTED_COST_OBJECTIVE_H

#include "probfd/analysis_objectives/analysis_objective.h"

#include <memory>

namespace probfd {

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
    virtual GlobalRewardFunction* reward() override;

private:
    std::unique_ptr<GlobalRewardFunction> reward_;
};

} // namespace analysis_objectives
} // namespace probfd

#endif // __EXPECTED_COST_OBJECTIVE_H__