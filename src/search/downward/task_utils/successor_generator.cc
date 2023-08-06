#include "downward/task_utils/successor_generator.h"

#include "downward/task_utils/successor_generator_factory.h"
#include "downward/task_utils/successor_generator_internals.h"

#include "downward/abstract_task.h"

#include "probfd/task_state_space.h"
#include "probfd/transition.h"

using namespace std;

namespace successor_generator {
SuccessorGenerator::SuccessorGenerator(const TaskBaseProxy& task_proxy)
    : root(SuccessorGeneratorFactory(task_proxy).create())
{
}

SuccessorGenerator::~SuccessorGenerator() = default;

void SuccessorGenerator::generate_applicable_ops(
    const State& state,
    vector<OperatorID>& applicable_ops) const
{
    state.unpack();
    root->generate_applicable_ops(state.get_unpacked_values(), applicable_ops);
}

void SuccessorGenerator::generate_transitions(
    const State& state,
    std::vector<probfd::Transition<OperatorID>>& transitions,
    probfd::InducedTaskStateSpace& task_state_space) const
{
    state.unpack();
    root->generate_transitions(
        state,
        transitions,
        task_state_space);
}

PerTaskInformation<SuccessorGenerator> g_successor_generators;
} // namespace successor_generator
