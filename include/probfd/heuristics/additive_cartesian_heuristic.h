#ifndef PROBFD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H
#define PROBFD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace probfd::cartesian_abstractions {
class CartesianHeuristicFunction;
class FlawGeneratorFactory;
class SplitSelectorFactory;
class SubtaskGenerator;
} // namespace probfd::cartesian_abstractions

namespace probfd::heuristics {

/*
  Store CartesianHeuristicFunctions and compute overall heuristic by
  summing all of their values.
*/
class AdditiveCartesianHeuristic final : public FDREvaluator {
    std::vector<cartesian_abstractions::CartesianHeuristicFunction>
        heuristic_functions_;

public:
    explicit AdditiveCartesianHeuristic(
        std::vector<cartesian_abstractions::CartesianHeuristicFunction>
            heuristic_functions);

    AdditiveCartesianHeuristic(AdditiveCartesianHeuristic&&) noexcept;

    ~AdditiveCartesianHeuristic() override;

protected:
    value_t evaluate(const downward::State& ancestor_state) const override;
};

class AdditiveCartesianHeuristicFactory final : public TaskHeuristicFactory {
    const std::vector<std::shared_ptr<cartesian_abstractions::SubtaskGenerator>>
        subtask_generators;
    const std::shared_ptr<cartesian_abstractions::FlawGeneratorFactory>
        flaw_generator_factory;
    const std::shared_ptr<cartesian_abstractions::SplitSelectorFactory>
        split_selector_factory;
    const int max_states;
    const int max_transitions;
    const double max_time;
    const bool use_general_costs;

    mutable downward::utils::LogProxy log_;

public:
    AdditiveCartesianHeuristicFactory(
        std::vector<std::shared_ptr<cartesian_abstractions::SubtaskGenerator>>
            subtasks,
        std::shared_ptr<cartesian_abstractions::FlawGeneratorFactory>
            flaw_generator_factory,
        std::shared_ptr<cartesian_abstractions::SplitSelectorFactory>
            split_selector_factory,
        int max_states,
        int max_transitions,
        double max_time,
        bool use_general_costs,
        downward::utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator> create_heuristic(
        const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H
