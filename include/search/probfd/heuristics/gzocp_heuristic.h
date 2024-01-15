#ifndef PROBFD_HEURISTICS_GZOCP_HEURISTIC_H
#define PROBFD_HEURISTICS_GZOCP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <memory>
#include <vector>

namespace utils {
class RandomNumberGenerator;
class LogProxy;
}

namespace probfd {

namespace pdbs {
class ProbabilityAwarePatternDatabase;
class PatternCollectionGenerator;
} // namespace pdbs

namespace heuristics {

class GZOCPHeuristic : public TaskDependentHeuristic {
public:
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

private:
    const value_t termination_cost;
    const OrderingStrategy ordering;
    const std::shared_ptr<utils::RandomNumberGenerator> rng;

    std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs;

public:
    explicit GZOCPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        utils::LogProxy log,
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        OrderingStrategy order,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

    void print_statistics() const override
    {
        // TODO
    }

protected:
    value_t evaluate(const State& state) const override;
};

} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_GZOCP_HEURISTIC_H
