#ifndef PROBFD_HEURISTICS_CARTESIAN_COST_SATURATION_H
#define PROBFD_HEURISTICS_CARTESIAN_COST_SATURATION_H

#include "downward/cegar/refinement_hierarchy.h"

#include "probfd/heuristics/cartesian/split_selector.h"

#include "probfd/value_type.h"

#include <memory>
#include <vector>

namespace utils {
class CountdownTimer;
class Duration;
class RandomNumberGenerator;
class LogProxy;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace cartesian {

class CartesianHeuristicFunction;
class FlawGeneratorFactory;
class SubtaskGenerator;

/*
  Get subtasks from SubtaskGenerators, reduce their costs by wrapping
  them in ModifiedOperatorCostsTasks, compute Abstractions, move
  RefinementHierarchies from Abstractions to
  CartesianHeuristicFunctions, allow extracting
  CartesianHeuristicFunctions into AdditiveCartesianHeuristic.
*/
class CostSaturation {
    const std::vector<std::shared_ptr<SubtaskGenerator>> subtask_generators;
    const std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory;
    const int max_states;
    const int max_non_looping_transitions;
    const double max_time;
    const bool use_general_costs;
    const PickSplit pick_split;
    utils::RandomNumberGenerator& rng;
    utils::LogProxy& log;

    std::vector<CartesianHeuristicFunction> heuristic_functions;
    std::vector<value_t> remaining_costs;
    int num_abstractions;
    int num_states;
    int num_non_looping_transitions;

    void reset(const ProbabilisticTaskProxy& task_proxy);
    void reduce_remaining_costs(const std::vector<value_t>& saturated_costs);
    std::shared_ptr<ProbabilisticTask>
    get_remaining_costs_task(std::shared_ptr<ProbabilisticTask>& parent) const;
    bool state_is_dead_end(const State& state) const;
    void build_abstractions(
        const std::vector<std::shared_ptr<ProbabilisticTask>>& subtasks,
        const utils::CountdownTimer& timer,
        std::function<bool()> should_abort);
    void print_statistics(utils::Duration init_time) const;

public:
    CostSaturation(
        const std::vector<std::shared_ptr<SubtaskGenerator>>&
            subtask_generators,
        std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
        int max_states,
        int max_non_looping_transitions,
        double max_time,
        bool use_general_costs,
        PickSplit pick_split,
        utils::RandomNumberGenerator& rng,
        utils::LogProxy& log);

    std::vector<CartesianHeuristicFunction> generate_heuristic_functions(
        const std::shared_ptr<ProbabilisticTask>& task);
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
