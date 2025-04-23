#include "downward/task_utils/successor_generator.h"

#include "downward/task_utils/successor_generator_factory.h"
#include "downward/task_utils/successor_generator_internals.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

using namespace std;

namespace downward::successor_generator {
SuccessorGenerator::SuccessorGenerator(const PlanningTask& task)
    : root(SuccessorGeneratorFactory(task).create())
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

PerTaskInformation<SuccessorGenerator> g_successor_generators;
} // namespace successor_generator
