#ifndef PROBFD_REWARD_MODELS_SSP_REWARD_MODELS_H
#define PROBFD_REWARD_MODELS_SSP_REWARD_MODELS_H

#include "probfd/reward_model.h"

#include <memory>

namespace probfd {
namespace reward_models {

/**
 * @brief The SSP reward model.
 *
 * Implements the SSP reward model. The reward function for SSPs specifies
 * operator rewards according to the negative cost of the operator as read from
 * the input file. The termination reward for goal states is zero, whereas
 * termination in other states is not allowed (specified by negative infinity).
 *
 * It is assumed that the input task has non-negative operator costs, so all
 * state values lie in the range [-infinity, 0] (where -infinity is attained if
 * the goal cannot be reached with probability 1).
 */
class SSPRewardModel : public RewardModel {
    std::unique_ptr<TaskRewardFunction> reward_;

public:
    /// Default constructor.
    explicit SSPRewardModel();

    /// Returns the interval [-infinity, 0]
    virtual value_utils::IntervalValue reward_bound() override;

    virtual TaskRewardFunction* get_reward_function() override;
};

} // namespace reward_models
} // namespace probfd

#endif