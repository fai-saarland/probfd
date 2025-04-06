#ifndef PROBFD_HEURISTICS_UCP_HEURISTIC_H
#define PROBFD_HEURISTICS_UCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

namespace probfd::pdbs {
struct ProbabilityAwarePatternDatabase;
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class UCPHeuristic final : public FDREvaluator {
    const value_t termination_cost_;
    const std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs_;

public:
    UCPHeuristic(
        value_t termination_cost,
        std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs);

    ~UCPHeuristic() override;

    void print_statistics() const override
    {
        // TODO
    }

protected:
    value_t evaluate(const downward::State& state) const override;
};

class UCPHeuristicFactory final : public TaskHeuristicFactory {
    const downward::utils::Verbosity verbosity_;
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;

public:
    UCPHeuristicFactory(
        downward::utils::Verbosity verbosity,
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    std::unique_ptr<FDREvaluator> create_heuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_UCP_HEURISTIC_H
