#ifndef PROBFD_COST_MODEL_H
#define PROBFD_COST_MODEL_H

#include "probfd/engine_interfaces/cost_function.h"

#include "probfd/value_utils.h"

#include <memory>

namespace probfd {

/**
 * @brief Represents the cost model of the overall analysis.
 *
 * The cost model of the analysis is given by a cost function to be used
 * for the input planning task of the planner, as well as an interval of
 * possible state values.
 */
class CostModel {
public:
    virtual ~CostModel() = default;

    /**
     * @brief Returns an closed interval in which the optimal state values lie
     * (with respect to the cost function).
     *
     * The cost model may specify infinite upper or lower bounds in case the
     * state values may be infinite or if a conservative bound is not available.
     */
    virtual Interval optimal_value_bound() const = 0;

    /// Returns a cost function for the input task.
    virtual TaskCostFunction* get_cost_function() = 0;
};

extern std::shared_ptr<CostModel> g_cost_model;

} // namespace probfd

#endif // ANALYSIS_OBJECTIVE_H