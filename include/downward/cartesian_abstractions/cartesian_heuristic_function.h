#ifndef CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H
#define CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H

#include <memory>
#include <vector>

namespace downward {
class AbstractTask;
class State;
}

namespace downward::cartesian_abstractions {
class RefinementHierarchy;
/*
  Store RefinementHierarchy and heuristic values for looking up abstract state
  IDs and corresponding heuristic values efficiently.
*/
class CartesianHeuristicFunction {
    // Avoid const to enable moving.
    std::shared_ptr<AbstractTask> task;
    std::unique_ptr<RefinementHierarchy> refinement_hierarchy;
    std::vector<int> h_values;

public:
    CartesianHeuristicFunction(
        std::shared_ptr<AbstractTask> task,
        std::unique_ptr<RefinementHierarchy>&& hierarchy,
        std::vector<int>&& h_values);

    CartesianHeuristicFunction(const CartesianHeuristicFunction&) = delete;
    CartesianHeuristicFunction(CartesianHeuristicFunction&&) = default;

    int get_value(const State& state) const;
};
} // namespace cartesian_abstractions

#endif
