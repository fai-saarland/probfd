#include "probfd/task_utils/probabilistic_successor_generator.h"

#include "probfd/task_utils/probabilistic_successor_generator_factory.h"
#include "probfd/task_utils/probabilistic_successor_generator_internals.h"

#include "downward/abstract_task.h"

#include "probfd/task_state_space.h"
#include "probfd/transition.h"

using namespace std;

namespace probfd {
namespace successor_generator {

ProbabilisticSuccessorGenerator::ProbabilisticSuccessorGenerator(
    const TaskBaseProxy& task_proxy)
    : root(ProbabilisticSuccessorGeneratorFactory(task_proxy).create())
{
}

ProbabilisticSuccessorGenerator::~ProbabilisticSuccessorGenerator() = default;

void ProbabilisticSuccessorGenerator::generate_applicable_ops(
    const State& state,
    vector<OperatorID>& applicable_ops) const
{
    state.unpack();
    root->generate_applicable_ops(state.get_unpacked_values(), applicable_ops);
}

void ProbabilisticSuccessorGenerator::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    state.unpack();
    root->generate_transitions(state, transitions, task_state_space);
}

} // namespace successor_generator
} // namespace probfd