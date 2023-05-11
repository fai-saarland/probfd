#ifndef PROBFD_HEURISTICS_CARTESIAN_ADDITIVE_CARTESIAN_HEURISTIC_H
#define PROBFD_HEURISTICS_CARTESIAN_ADDITIVE_CARTESIAN_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

class CartesianHeuristicFunction;

/*
  Store CartesianHeuristicFunctions and compute overall heuristic by
  summing all of their values.
*/
class AdditiveCartesianHeuristic : public TaskDependentHeuristic {
    const std::vector<CartesianHeuristicFunction> heuristic_functions;

protected:
    EvaluationResult evaluate(const State& ancestor_state) const override;

public:
    explicit AdditiveCartesianHeuristic(const options::Options& opts);
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
