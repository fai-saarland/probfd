#include "downward/task_utils/successor_generator.h"

#include "downward/task_utils/successor_generator_factory.h"
#include "downward/task_utils/successor_generator_internals.h"

#include "downward/abstract_task.h"

using namespace std;

namespace downward::successor_generator {
SuccessorGenerator::SuccessorGenerator(const PlanningTaskProxy& task_proxy)
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

PerTaskInformation<SuccessorGenerator> g_successor_generators;
} // namespace successor_generator
