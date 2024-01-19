#ifndef PROBFD_TASK_COST_FUNCTION_FACTORY_H
#define PROBFD_TASK_COST_FUNCTION_FACTORY_H

#include "probfd/fdr_types.h"

#include <memory>

// Forward Declarations
namespace probfd {
class ProbabilisticTask;
}

namespace probfd {

class TaskCostFunctionFactory {
public:
    virtual ~TaskCostFunctionFactory() = default;

    virtual std::unique_ptr<FDRCostFunction>
    create_cost_function(std::shared_ptr<ProbabilisticTask> task) = 0;
};

} // namespace probfd

#endif // PROBFD_TASK_COST_FUNCTION_FACTORY_H