#ifndef PROBFD_COST_MODELS_MAXPROB_COST_MODEL_H
#define PROBFD_COST_MODELS_MAXPROB_COST_MODEL_H

#include "probfd/cost_model.h"

#include <memory>

namespace probfd {

/// This namespace contains cost model specifications.
namespace cost_models {

/**
 * @brief The MaxProb cost model.
 *
 * Implements the MaxProb cost model. The cost function for MaxProb
 * specifies a termination cost of 0 for goal states and 1 for non-goal
 * states. Actions have zero cost. An optimal state value is 1 minus the
 * maximal probability of reaching the goal.
 */
class MaxProbCostModel : public CostModel {
    std::unique_ptr<TaskCostFunction> cost_function_;

public:
    /// Default constructor.
    explicit MaxProbCostModel();

    /// Returns the interval [0, 1]
    virtual Interval optimal_value_bound() const override;

    virtual TaskCostFunction* get_cost_function() override;
};

} // namespace cost_models
} // namespace probfd

#endif