#pragma once

#include "../action_evaluator.h"
#include "../state_reward_function.h"
#include "../value_type.h"

#include <memory>

namespace probabilistic {

/// Namespace dedicated to MDP planning analysis objectives
namespace analysis_objectives {

/**
 * @brief The MDP analysis objective.
 *
 * Configures the reward of a transition in the MDP model.
 * The reward is specified by:
 * - A \ref GlobalStateRewardFunction specifiying the immediate reward
 * when transitioning to some successor state, independent of source
 * and action.
 * - A \ref ProbabilisticOperatorEvaluator specifying the immediate
 * reward gained by applying an action in a given source state, independent
 * of possible successor states.
 *
 * Additionally, each analysis objective specifies an upper and a lower bound
 * on the possible state values (-/+ infinity if no such bounds exist)
 *
 */
class AnalysisObjective {
public:
    virtual ~AnalysisObjective() = default;

    /// Returns a lower bound on the state values for this analysis objective.
    virtual value_type::value_t min() = 0;

    /// Returns an upper bound on the state values for this analysis objective.
    virtual value_type::value_t max() = 0;

    /// Returns a GlobalStateRewardFunction specifying the state rewards.
    virtual GlobalStateRewardFunction* state_reward() = 0;

    /// Returns a ProbabilisticOperatorEvaluator specifying the action rewards.
    virtual ProbabilisticOperatorEvaluator* action_reward() = 0;
};

} // namespace analysis_objectives
} // namespace probabilistic
