#ifndef PROBFD_HEURISTICS_SCP_HEURISTIC_H
#define PROBFD_HEURISTICS_SCP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace utils {
class RandomNumberGenerator;
class LogProxy;
} // namespace utils

namespace probfd::pdbs {
class ProbabilityAwarePatternDatabase;
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class SCPHeuristic : public TaskDependentHeuristic {
public:
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

private:
    const value_t termination_cost_;
    const OrderingStrategy ordering_;
    const std::shared_ptr<utils::RandomNumberGenerator> rng_;

    std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs_;

public:
    explicit SCPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        utils::LogProxy log,
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        OrderingStrategy order,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    value_t evaluate(const State& state) const override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_SCP_HEURISTIC_H
