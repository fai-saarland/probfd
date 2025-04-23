#ifndef TASK_TRANSFORMATION_H
#define TASK_TRANSFORMATION_H

#include <memory>
#include <vector>

namespace downward {
class AbstractTask;
class State;
class OperatorID;
}

namespace downward {

class StateMapping {
public:
    virtual ~StateMapping() = default;

    /*
     * Convert state values from the original task into the transformed task.
     * The values are converted in-place to avoid unnecessary copies. If a
     * subclass needs to create a new vector, e.g., because the size changes,
     * it should create the new vector in a local variable and then swap it
     * with the parameter.
     */
    virtual void
    convert_ancestor_state_values(std::vector<int>& values) const = 0;

    State convert_ancestor_state(const State& ancestor_state) const;
};

class InverseOperatorMapping {
public:
    virtual ~InverseOperatorMapping() = default;

    /*
     * Convert an operator index from the transformed task into an operator
     * index from the original task.
     */
    virtual int convert_operator_index(int index) const = 0;

    OperatorID get_ancestor_operator_id(int index) const;
};

struct TaskTransformationResult {
    std::shared_ptr<AbstractTask> transformed_task;
    std::shared_ptr<StateMapping> state_mapping;
    std::shared_ptr<InverseOperatorMapping> inv_operator_mapping;
};

class TaskTransformation {
public:
    virtual ~TaskTransformation() = default;

    virtual TaskTransformationResult
    transform(const std::shared_ptr<AbstractTask>& original_task) = 0;
};

}

#endif // TASK_TRANSFORMATION_H
