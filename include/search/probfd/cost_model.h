#ifndef PROBFD_COST_MODEL_H
#define PROBFD_COST_MODEL_H

#include "probfd/interval.h"
#include "probfd/task_types.h"

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

    /// Returns a cost function for the input task.
    virtual TaskSimpleCostFunction* get_cost_function() = 0;
};

extern std::shared_ptr<CostModel> g_cost_model;

} // namespace probfd

#endif // ANALYSIS_OBJECTIVE_H