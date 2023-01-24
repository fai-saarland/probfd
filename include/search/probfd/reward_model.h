#ifndef PROBFD_REWARD_MODEL_H
#define PROBFD_REWARD_MODEL_H

#include "probfd/engine_interfaces/reward_function.h"

#include "probfd/value_utils.h"

#include <memory>

namespace probfd {

/**
 * @brief Represents the reward model of the overall analysis.
 *
 * The reward model of the analysis is given by a reward function to be used
 * for the input planning task of the planner, as well as an interval of
 * possible state values.
 */
class RewardModel {
public:
    virtual ~RewardModel() = default;

    /**
     * @brief Returns an closed interval in which the optimal state values lie
     * (with respect to the reward function).
     *
     * The reward model may specify infinite upper or lower bounds in case the
     * state values may be infinite or if a conservative bound is not available.
     */
    virtual Interval reward_bound() = 0;

    /// Returns a reward function to be used to analyze the input task.
    virtual TaskRewardFunction* get_reward_function() = 0;
};

extern std::shared_ptr<RewardModel> g_reward_model;

} // namespace probfd

#endif // ANALYSIS_OBJECTIVE_H