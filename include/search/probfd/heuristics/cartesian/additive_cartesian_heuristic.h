#ifndef PROBFD_HEURISTICS_CARTESIAN_ADDITIVE_CARTESIAN_HEURISTIC_H
#define PROBFD_HEURISTICS_CARTESIAN_ADDITIVE_CARTESIAN_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

class CartesianHeuristicFunction;
class FlawGeneratorFactory;
class SplitSelectorFactory;
class SubtaskGenerator;

/*
  Store CartesianHeuristicFunctions and compute overall heuristic by
  summing all of their values.
*/
class AdditiveCartesianHeuristic : public TaskDependentHeuristic {
    const std::vector<CartesianHeuristicFunction> heuristic_functions;

public:
    AdditiveCartesianHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        std::vector<std::shared_ptr<SubtaskGenerator>> subtask_generators,
        std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
        std::shared_ptr<SplitSelectorFactory> split_selector_factory,
        int max_states,
        int max_transitions,
        double max_time,
        bool use_general_costs);

protected:
    EvaluationResult evaluate(const State& ancestor_state) const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
