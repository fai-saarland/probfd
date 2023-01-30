#ifndef MDPS_ANALYSIS_OBJECTIVES_ANALYSIS_OBJECTIVE_H
#define MDPS_ANALYSIS_OBJECTIVES_ANALYSIS_OBJECTIVE_H

#include "probfd/engine_interfaces/reward_function.h"

#include "probfd/value_utils.h"

#include <memory>

namespace probfd {

/// Namespace dedicated to MDP planning analysis objectives
namespace analysis_objectives {

/**
 * @brief The MDP analysis objective.
 *
 * Configures the reward of a transition in the MDP model.
 * The reward is specified by a TaskRewardFunction.
 * Additionally, each analysis objective specifies an upper and a lower bound
 * on the possible state values (-/+ infinity if no such bounds exist).
 */
class AnalysisObjective {
public:
    virtual ~AnalysisObjective() = default;

    /// Returns a lower bound on the state values for this analysis objective.
    virtual value_utils::IntervalValue reward_bound() = 0;

    /// Returns a GlobalRewardFunction specifying the rewards.
    virtual TaskRewardFunction* reward() = 0;
};

} // namespace analysis_objectives

extern std::shared_ptr<analysis_objectives::AnalysisObjective>
    g_analysis_objective;
} // namespace probfd

#endif // ANALYSIS_OBJECTIVE_H