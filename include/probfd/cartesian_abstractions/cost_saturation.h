#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_COST_SATURATION_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_COST_SATURATION_H

#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <functional>
#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class State;
}

namespace downward::utils {
class CountdownTimer;
class Duration;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
class ProbabilisticTask;
} // namespace probfd

namespace probfd::cartesian_abstractions {
class CartesianHeuristicFunction;
class FlawGeneratorFactory;
class SplitSelectorFactory;
class SubtaskGenerator;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/*
  Get subtasks from SubtaskGenerators, reduce their costs by wrapping
  them in ModifiedOperatorCostsTasks, compute Abstractions, move
  RefinementHierarchies from Abstractions to
  CartesianHeuristicFunctions, allow extracting
  CartesianHeuristicFunctions into AdditiveCartesianHeuristic.
*/
class CostSaturation {
    const std::vector<std::shared_ptr<SubtaskGenerator>> subtask_generators_;
    const std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory_;
    const std::shared_ptr<SplitSelectorFactory> split_selector_factory_;
    const int max_states_;
    const int max_non_looping_transitions_;
    const double max_time_;
    const bool use_general_costs_;
    mutable downward::utils::LogProxy log_;

    std::vector<CartesianHeuristicFunction> heuristic_functions_;
    std::vector<value_t> remaining_costs_;
    int num_abstractions_;
    int num_states_;
    int num_non_looping_transitions_;

    void reset(const ProbabilisticTaskProxy& task_proxy);
    void reduce_remaining_costs(const std::vector<value_t>& saturated_costs);
    std::shared_ptr<ProbabilisticTask>
    get_remaining_costs_task(std::shared_ptr<ProbabilisticTask>& parent) const;
    bool state_is_dead_end(const downward::State& state) const;
    void build_abstractions(
        const std::vector<std::shared_ptr<ProbabilisticTask>>& subtasks,
        const downward::utils::CountdownTimer& timer,
        std::function<bool()> should_abort);
    void print_statistics(downward::utils::Duration init_time) const;

public:
    CostSaturation(
        const std::vector<std::shared_ptr<SubtaskGenerator>>&
            subtask_generators,
        std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
        std::shared_ptr<SplitSelectorFactory> split_selector_factory,
        int max_states,
        int max_non_looping_transitions,
        double max_time,
        bool use_general_costs,
        downward::utils::LogProxy log);

    ~CostSaturation();

    std::vector<CartesianHeuristicFunction> generate_heuristic_functions(
        const std::shared_ptr<ProbabilisticTask>& task);
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_COST_SATURATION_H
