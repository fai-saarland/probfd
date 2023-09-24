#include "downward/task_proxy.h"

#include "downward/axioms.h"
#include "downward/state_registry.h"

#include "downward/task_utils/causal_graph.h"
#include "downward/task_utils/task_properties.h"

#include <iostream>

using namespace std;

State::State(
    const AbstractTaskBase& task,
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer)
    : task(&task)
    , registry(&registry)
    , id(id)
    , buffer(buffer)
    , values(nullptr)
    , state_packer(&registry.get_state_packer())
    , num_variables(registry.get_num_variables())
{
    assert(id != StateID::no_state);
    assert(buffer);
    assert(num_variables == task.get_num_variables());
}

State::State(
    const AbstractTaskBase& task,
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer,
    vector<int>&& values)
    : State(task, registry, id, buffer)
{
    assert(num_variables == static_cast<int>(values.size()));
    this->values = make_shared<vector<int>>(std::move(values));
}

State::State(const AbstractTaskBase& task, vector<int>&& values)
    : task(&task)
    , registry(nullptr)
    , id(StateID::no_state)
    , buffer(nullptr)
    , values(make_shared<vector<int>>(std::move(values)))
    , state_packer(nullptr)
    , num_variables(this->values->size())
{
    assert(num_variables == task.get_num_variables());
}

State State::get_unregistered_successor(const OperatorProxy& op) const
{
    assert(task_properties::is_applicable(op, *this));
    return get_unregistered_successor(op.get_effects());
}

void State::apply_axioms(std::vector<int>& values) const
{
    if (task->get_num_axioms() > 0) {
        AxiomEvaluator& axiom_evaluator =
            g_axiom_evaluators[TaskBaseProxy(*task)];
        axiom_evaluator.evaluate(values);
    }
}

const causal_graph::CausalGraph& TaskProxy::get_causal_graph() const
{
    return causal_graph::get_causal_graph(
        static_cast<const AbstractTask*>(task));
}
