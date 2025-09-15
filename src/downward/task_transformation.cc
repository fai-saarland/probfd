#include "downward/task_transformation.h"

#include "downward/operator_id.h"
#include "downward/state.h"

namespace downward {

State StateMapping::convert_ancestor_state(const State& ancestor_state) const
{
    ancestor_state.unpack();
    std::vector<int> state_values = ancestor_state.get_unpacked_values();
    this->convert_ancestor_state_values(state_values);
    return State(std::move(state_values));
}

OperatorID InverseOperatorMapping::get_ancestor_operator_id(int index) const
{
    return OperatorID(this->convert_operator_index(index));
}

}