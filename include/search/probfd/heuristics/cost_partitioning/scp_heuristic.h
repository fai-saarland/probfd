#ifndef PROBFD_HEURISTICS_PDBS_SATURATED_COST_PARTITIONING_SCP_HEURISTIC_H
#define PROBFD_HEURISTICS_PDBS_SATURATED_COST_PARTITIONING_SCP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_collection_generator.h"

#include "probfd/evaluator.h"
#include "probfd/types.h"

#include <memory>
#include <vector>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProbabilityAwarePatternDatabase;

class SCPHeuristic : public TaskDependentHeuristic {
public:
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

private:
    std::vector<ProbabilityAwarePatternDatabase> pdbs;
    OrderingStrategy ordering;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit SCPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<TaskCostFunction> task_cost_function,
        utils::LogProxy log,
        std::shared_ptr<PatternCollectionGenerator> generator,
        OrderingStrategy order,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    EvaluationResult evaluate(const State& state) const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif