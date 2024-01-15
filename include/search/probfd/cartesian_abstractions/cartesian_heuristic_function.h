#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H

#include "probfd/cartesian_abstractions/types.h"

#include "probfd/value_type.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include <memory>
#include <vector>

class State;

namespace probfd {
namespace cartesian_abstractions {

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

    ~CartesianHeuristicFunction();

    value_t get_value(const State& state) const;
};

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_HEURISTIC_FUNCTION_H
