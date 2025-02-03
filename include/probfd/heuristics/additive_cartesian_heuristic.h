#ifndef PROBFD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H
#define PROBFD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "probfd/task_heuristic_factory.h"

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
class AdditiveCartesianHeuristic : public TaskDependentHeuristic {
    const std::vector<cartesian_abstractions::CartesianHeuristicFunction>
        heuristic_functions_;

public:
    AdditiveCartesianHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        std::vector<std::shared_ptr<cartesian_abstractions::SubtaskGenerator>>
            subtask_generators,
        std::shared_ptr<cartesian_abstractions::FlawGeneratorFactory>
            flaw_generator_factory,
        std::shared_ptr<cartesian_abstractions::SplitSelectorFactory>
            split_selector_factory,
        int max_states,
        int max_transitions,
        double max_time,
        bool use_general_costs);

protected:
    value_t evaluate(const State& ancestor_state) const override;
};

class AdditiveCartesianHeuristicFactory : public TaskHeuristicFactory {
    const std::vector<
        std::shared_ptr<probfd::cartesian_abstractions ::SubtaskGenerator>>
        subtask_generators;
    const std::shared_ptr<probfd::cartesian_abstractions ::FlawGeneratorFactory>
        flaw_generator_factory;
    const std::shared_ptr<probfd::cartesian_abstractions ::SplitSelectorFactory>
        split_selector_factory;
    const int max_states;
    const int max_transitions;
    const double max_time;
    const bool use_general_costs;

    const utils::LogProxy log_;

public:
    AdditiveCartesianHeuristicFactory(
        std::vector<std::shared_ptr<
            probfd::cartesian_abstractions ::SubtaskGenerator>> subtasks,
        std::shared_ptr<probfd::cartesian_abstractions ::FlawGeneratorFactory>
            flaw_generator_factory,
        std::shared_ptr<probfd::cartesian_abstractions ::SplitSelectorFactory>
            split_selector_factory,
        int max_states,
        int max_transitions,
        double max_time,
        bool use_general_costs,
        utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_ADDITIVE_CARTESIAN_HEURISTIC_H
