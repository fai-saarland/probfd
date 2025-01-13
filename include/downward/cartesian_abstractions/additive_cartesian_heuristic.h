#ifndef CEGAR_ADDITIVE_CARTESIAN_HEURISTIC_H
#define CEGAR_ADDITIVE_CARTESIAN_HEURISTIC_H

#include "downward/heuristic.h"

#include <vector>

namespace cartesian_abstractions {
class CartesianHeuristicFunction;
class SubtaskGenerator;
enum class PickSplit;

/*
  Store CartesianHeuristicFunctions and compute overall heuristic by
  summing all of their values.
*/
class AdditiveCartesianHeuristic : public Heuristic {
    const std::vector<CartesianHeuristicFunction> heuristic_functions;

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    AdditiveCartesianHeuristic(
        const std::vector<std::shared_ptr<SubtaskGenerator>>& subtasks,
        int max_states,
        int max_transitions,
        double max_time,
        PickSplit pick,
        bool use_general_costs,
        int random_seed,
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    AdditiveCartesianHeuristic(
        const std::vector<std::shared_ptr<SubtaskGenerator>>& subtasks,
        int max_states,
        int max_transitions,
        double max_time,
        PickSplit pick,
        bool use_general_costs,
        int random_seed,
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);
};
} // namespace cartesian_abstractions

#endif
