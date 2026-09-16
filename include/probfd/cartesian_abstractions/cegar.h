#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_CEGAR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_CEGAR_H

#include "probfd/cartesian_abstractions/types.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class VariableSpace;
}

namespace downward::utils {
class Timer;
} // namespace downward::utils

namespace probfd::cartesian_abstractions {
class AbstractState;
class CartesianAbstraction;
class CartesianHeuristic;
struct Flaw;
class FlawGenerator;
class SplitSelector;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/**
 * Contains the final abstraction mapping (RefinementHierarchy), the final
 * Cartesian abstraction, and the final heuristic for the abstract state
 * distances.
 */
struct CEGARResult {
    std::unique_ptr<RefinementHierarchy> refinement_hierarchy;
    std::unique_ptr<CartesianAbstraction> abstraction;
    std::unique_ptr<CartesianHeuristic> heuristic;

    ~CEGARResult();
};

CEGARResult run_refinement_loop(
    int max_states,
    int max_non_looping_transitions,
    downward::utils::FSeconds max_time,
    FlawGenerator& flaw_generator_factory,
    SplitSelector& split_selector_factory,
    const downward::utils::LogProxy& log,
    const ProbabilisticTaskTuple& task);

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_CEGAR_H
