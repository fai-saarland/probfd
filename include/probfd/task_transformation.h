#ifndef PROBFD_TASK_TRANSFORMATION_H
#define PROBFD_TASK_TRANSFORMATION_H

#include "downward/task_transformation.h"

#include "probfd/probabilistic_task.h"

#include <memory>
#include <vector>

namespace downward {
class State;
class OperatorID;
}

namespace probfd {

struct ProbabilisticTaskTransformationResult {
    SharedProbabilisticTask transformed_task;
    std::shared_ptr<downward::StateMapping> state_mapping;

    // Not needed at the moment
    // std::shared_ptr<downward::InverseOperatorMapping> inv_operator_mapping;
};

class ProbabilisticTaskTransformation {
public:
    virtual ~ProbabilisticTaskTransformation() = default;

    virtual ProbabilisticTaskTransformationResult
    transform(const SharedProbabilisticTask& original_task) = 0;
};

}

#endif
