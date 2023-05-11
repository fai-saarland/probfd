#ifndef PROBFD_HEURISTICS_CARTESIAN_CARTESIAN_HEURISTIC_FUNCTION_H
#define PROBFD_HEURISTICS_CARTESIAN_CARTESIAN_HEURISTIC_FUNCTION_H

#include "probfd/heuristics/cartesian/types.h"

#include "probfd/value_type.h"

#include <memory>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace cartesian {

/*
  Store RefinementHierarchy and heuristic values for looking up abstract state
  IDs and corresponding heuristic values efficiently.
*/
class CartesianHeuristicFunction {
    // Avoid const to enable moving.
    std::unique_ptr<RefinementHierarchy> refinement_hierarchy;
    std::vector<value_t> h_values;

public:
    CartesianHeuristicFunction(
        std::unique_ptr<RefinementHierarchy>&& hierarchy,
        std::vector<value_t>&& h_values);

    CartesianHeuristicFunction(const CartesianHeuristicFunction&) = delete;
    CartesianHeuristicFunction(CartesianHeuristicFunction&&) = default;

    value_t get_value(const State& state) const;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
