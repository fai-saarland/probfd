#ifndef TRANSFORMATIONS_IDENTITY_TRANSFORMATION_H
#define TRANSFORMATIONS_IDENTITY_TRANSFORMATION_H

#include "downward/task_transformation.h"

namespace downward {

class IdentityStateMapping : public StateMapping {
public:
    void convert_ancestor_state_values(std::vector<int>&) const override;
};

class IdentityOperatorMapping : public InverseOperatorMapping {
public:
    int convert_operator_index(int index) const override;
};

class IdentityTaskTransformation : public TaskTransformation {
public:
    TaskTransformationResult
    transform(const std::shared_ptr<AbstractTask>& original_task) override;
};

}

#endif // TRANSFORMATIONS_IDENTITY_TRANSFORMATION_H
