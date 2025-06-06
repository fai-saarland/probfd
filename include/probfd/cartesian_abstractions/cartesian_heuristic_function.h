#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H

#include "probfd/cartesian_abstractions/types.h"

#include "probfd/value_type.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class State;
class StateMapping;
} // namespace downward

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::cartesian_abstractions {

/*
  Store RefinementHierarchy and heuristic values for looking up abstract state
  IDs and corresponding heuristic values efficiently.
*/
class CartesianHeuristicFunction {
    // Avoid const to enable moving.
    std::shared_ptr<downward::StateMapping> state_mapping_;
    std::shared_ptr<ProbabilisticTask> task_;
    std::unique_ptr<RefinementHierarchy> refinement_hierarchy_;
    std::vector<value_t> h_values_;

public:
    CartesianHeuristicFunction(
        std::shared_ptr<downward::StateMapping> state_mapping,
        std::shared_ptr<ProbabilisticTask> task,
        std::unique_ptr<RefinementHierarchy>&& hierarchy,
        std::vector<value_t>&& h_values);

    CartesianHeuristicFunction(const CartesianHeuristicFunction&) = delete;
    CartesianHeuristicFunction(CartesianHeuristicFunction&&) noexcept;

    CartesianHeuristicFunction&
    operator=(const CartesianHeuristicFunction&) = delete;
    CartesianHeuristicFunction&
    operator=(CartesianHeuristicFunction&&) noexcept;

    ~CartesianHeuristicFunction();

    [[nodiscard]]
    value_t get_value(const downward::State& state) const;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H
