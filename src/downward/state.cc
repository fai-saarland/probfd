#include "downward/state.h"

#include "downward/axioms.h"
#include "downward/state_registry.h"

#include "downward/task_utils/task_properties.h"

#include <iostream>

using namespace std;

namespace downward {

State::State(
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer)
    : registry(&registry)
    , id(id)
    , buffer(buffer)
    , values(nullptr)
    , state_packer(&registry.get_state_packer())
    , num_variables(registry.get_num_variables())
{
    assert(id != StateID::no_state);
    assert(buffer);
}

State::State(
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer,
    vector<int>&& values)
    : State(registry, id, buffer)
{
    assert(num_variables == static_cast<int>(values.size()));
    this->values = make_shared<vector<int>>(std::move(values));
}

State::State(vector<int>&& values)
    : registry(nullptr)
    , id(StateID::no_state)
    , buffer(nullptr)
    , values(make_shared<vector<int>>(std::move(values)))
    , state_packer(nullptr)
    , num_variables(this->values->size())
{
}

} // namespace downward