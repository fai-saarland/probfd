#ifndef PROBFD_HEURISTICS_PDBS_SATURATED_COST_PARTITIONING_UCP_HEURISTIC_H
#define PROBFD_HEURISTICS_PDBS_SATURATED_COST_PARTITIONING_UCP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_collection_generator.h"

#include "probfd/evaluator.h"
#include "probfd/types.h"

#include <memory>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {

class UCPHeuristic : public TaskDependentHeuristic {
    const value_t termination_cost;
    std::vector<ProbabilityAwarePatternDatabase> pdbs;

public:
    explicit UCPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<TaskCostFunction> task_cost_function,
        utils::LogProxy log,
        std::shared_ptr<PatternCollectionGenerator> generator);

    void print_statistics() const override
    {
        // TODO
    }

protected:
    value_t evaluate(const State& state) const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif