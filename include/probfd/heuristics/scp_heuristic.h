#ifndef PROBFD_HEURISTICS_SCP_HEURISTIC_H
#define PROBFD_HEURISTICS_SCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace downward::utils {
class RandomNumberGenerator;
class LogProxy;
} // namespace utils

namespace probfd::pdbs {
struct ProbabilityAwarePatternDatabase;
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class SCPHeuristic final : public FDREvaluator {
    const value_t termination_cost_;
    const std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs_;

public:
    explicit SCPHeuristic(
        value_t termination_cost,
        std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs);

    ~SCPHeuristic() override;

protected:
    value_t evaluate(const downward::State& state) const override;
};

class SCPHeuristicFactory final : public TaskHeuristicFactory {
public:
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

private:
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;
    const OrderingStrategy ordering_;
    const int random_seed_;
    const downward::utils::Verbosity verbosity_;

public:
    SCPHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator>
            pattern_collection_generator,
        OrderingStrategy ordering,
        int random_seed,
        downward::utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator> create_heuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_SCP_HEURISTIC_H
