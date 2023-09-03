#ifndef PROBFD_HEURISTICS_CARTESIAN_CARTESIAN_ABSTRACTION_INFO_H
#define PROBFD_HEURISTICS_CARTESIAN_CARTESIAN_ABSTRACTION_INFO_H

#include "probfd/heuristics/cartesian/types.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace cartesian {

class Abstraction;
class CartesianHeuristic;

struct CartesianAbstractionInfo {
    std::unique_ptr<RefinementHierarchy> refinement_hierarchy;
    std::unique_ptr<Abstraction> abstraction;
    std::unique_ptr<CartesianHeuristic> heuristic;

    ~CartesianAbstractionInfo();
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif