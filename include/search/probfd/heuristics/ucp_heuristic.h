#ifndef PROBFD_HEURISTICS_UCP_HEURISTIC_H
#define PROBFD_HEURISTICS_UCP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "probfd/evaluator.h"
#include "probfd/types.h"

#include <memory>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {

namespace pdbs {
class ProbabilityAwarePatternDatabase;
class PatternCollectionGenerator;
} // namespace pdbs

namespace heuristics {

class UCPHeuristic : public TaskDependentHeuristic {
    const value_t termination_cost;
    std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs;

public:
    explicit UCPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        utils::LogProxy log,
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    void print_statistics() const override
    {
        // TODO
    }

protected:
    value_t evaluate(const State& state) const override;
};

} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_UCP_HEURISTIC_H
