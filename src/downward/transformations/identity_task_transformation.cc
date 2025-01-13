#include "downward/transformations/identity_transformation.h"

namespace downward {

void IdentityStateMapping::convert_ancestor_state_values(
    std::vector<int>&) const
{
}

int IdentityOperatorMapping::convert_operator_index(int index) const
{
    return index;
}

TaskTransformationResult IdentityTaskTransformation::transform(
    const std::shared_ptr<AbstractTask>& original_task)
{
    return {
        original_task,
        std::make_shared<IdentityStateMapping>(),
        std::make_shared<IdentityOperatorMapping>()};
}

}