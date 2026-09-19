#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_CEGAR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_CEGAR_H

#include "probfd/cartesian_abstractions/types.h"

#include "probfd/probabilistic_task.h"

#include "downward/utils/durations.h"
#include "downward/utils/logging.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace probfd::cartesian_abstractions {
class CartesianAbstraction;
class CartesianHeuristic;
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

    CEGARResult(
        std::unique_ptr<RefinementHierarchy> refinement_hierarchy,
        std::unique_ptr<CartesianAbstraction> abstraction,
        std::unique_ptr<CartesianHeuristic> heuristic);

    CEGARResult(CEGARResult&&) noexcept;

    CEGARResult& operator=(CEGARResult&&) noexcept;

    ~CEGARResult();
};

CEGARResult run_refinement_loop(
    int max_states,
    int max_non_looping_transitions,
    downward::utils::FSeconds max_time,
    FlawGenerator& flaw_generator,
    SplitSelector& split_selector,
    const downward::utils::LogProxy& log,
    const ProbabilisticTaskTuple& task);

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_CEGAR_H
