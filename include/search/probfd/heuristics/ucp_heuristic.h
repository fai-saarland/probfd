#ifndef PROBFD_HEURISTICS_UCP_HEURISTIC_H
#define PROBFD_HEURISTICS_UCP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <memory>
#include <vector>

namespace probfd::pdbs {
class ProbabilityAwarePatternDatabase;
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class UCPHeuristic : public TaskDependentHeuristic {
    const value_t termination_cost_;
    std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs_;

public:
    explicit UCPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        utils::LogProxy log,
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    ~UCPHeuristic() override;

    void print_statistics() const override
    {
        // TODO
    }

protected:
    value_t evaluate(const State& state) const override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_UCP_HEURISTIC_H
