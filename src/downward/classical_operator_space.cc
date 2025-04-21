#include "downward/classical_operator_space.h"

#include "downward/task_utils/task_properties.h"

namespace downward {

void ClassicalOperatorProxy::apply_effect(std::vector<int>& values) const
{
    return state.get_unregistered_successor(task_proxy, get_effects());
}

}