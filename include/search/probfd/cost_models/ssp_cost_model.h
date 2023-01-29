#ifndef PROBFD_COST_MODELS_SSP_COST_MODELS_H
#define PROBFD_COST_MODELS_SSP_COST_MODELS_H

#include "probfd/cost_model.h"

#include <memory>

namespace probfd {
namespace cost_models {

/**
 * @brief The SSP cost model.
 *
 * Implements the SSP cost model. The cost function for SSPs specifies
 * operator costs according to the costs of the task operators as read from
 * the input file. The termination cost for goal states is zero, whereas
 * termination in other states is not allowed (specified by infinite cost).
 *
 * It is assumed that the input task has non-negative operator costs, so all
 * state values lie in the range [0, infinity] (where infinity is attained if
 * the goal cannot be reached with certainty).
 */
class SSPCostModel : public CostModel {
    std::unique_ptr<TaskCostFunction> cost_function_;

public:
    /// Default constructor.
    explicit SSPCostModel();

    /// Returns the interval [0, infinity]
    virtual Interval optimal_value_bound() const override;

    virtual TaskCostFunction* get_cost_function() override;
};

} // namespace cost_models
} // namespace probfd

#endif