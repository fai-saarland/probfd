#ifndef PROBFD_REWARD_MODELS_MAXPROB_REWARD_MODEL_H
#define PROBFD_REWARD_MODELS_MAXPROB_REWARD_MODEL_H

#include "probfd/reward_model.h"

#include <memory>

namespace probfd {

/// This namespace contains reward model specifications.
namespace reward_models {

/**
 * @brief The MaxProb reward model.
 *
 * Implements the MaxProb reward model. The reward function for MaxProb
 * specifies a termination reward of one for goal states and zero for non-goal
 * states. Actions have a reward of zero.
 *
 * All optimal state values are probabilities, i.e., in the interval [0, 1].
 */
class MaxProbRewardModel : public RewardModel {
    std::unique_ptr<TaskRewardFunction> reward_;

public:
    /// Default constructor.
    explicit MaxProbRewardModel();

    /// Returns the interval [0, 1]
    virtual Interval reward_bound() override;

    virtual TaskRewardFunction* get_reward_function() override;
};

} // namespace reward_models
} // namespace probfd

#endif