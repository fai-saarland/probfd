#include "probfd/task_utils/probabilistic_successor_generator.h"

#include "probfd/task_utils/probabilistic_successor_generator_factory.h"
#include "probfd/task_utils/probabilistic_successor_generator_internals.h"

#include "downward/task_proxy.h"

using namespace std;

namespace probfd::successor_generator {

ProbabilisticSuccessorGenerator::ProbabilisticSuccessorGenerator(
    const PlanningTaskProxy& task_proxy)
    : root_(ProbabilisticSuccessorGeneratorFactory(task_proxy).create())
{
}

ProbabilisticSuccessorGenerator::~ProbabilisticSuccessorGenerator() = default;

void ProbabilisticSuccessorGenerator::generate_applicable_ops(
    const State& state,
    vector<OperatorID>& applicable_ops) const
{
    state.unpack();
    root_->generate_applicable_ops(state.get_unpacked_values(), applicable_ops);
}

void ProbabilisticSuccessorGenerator::generate_transitions(
    const State& state,
    std::vector<TransitionTail<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    state.unpack();
    root_->generate_transitions(state, transitions, task_state_space);
}

} // namespace probfd::successor_generator
