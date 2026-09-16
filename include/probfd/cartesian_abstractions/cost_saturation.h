#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_COST_SATURATION_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_COST_SATURATION_H

#include "probfd/cartesian_abstractions/types.h"

#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <functional>
#include <memory>
#include <vector>

// Forward Declarations
namespace probfd::cartesian_abstractions {
class CartesianHeuristicFunction;
class FlawGeneratorFactory;
class SplitSelectorFactory;
class SubtaskGenerator;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

std::vector<CartesianHeuristicFunction> generate_heuristic_functions(
    const std::vector<std::shared_ptr<SubtaskGenerator>>& subtask_generators,
    FlawGeneratorFactory& flaw_generator_factory,
    SplitSelectorFactory& split_selector_factory,
    int max_states,
    int max_non_looping_transitions,
    downward::utils::FSeconds max_time,
    bool use_general_costs,
    downward::utils::LogProxy& log,
    const SharedProbabilisticTask& task);

} // namespace probfd::cartesian_abstractions

#endif
