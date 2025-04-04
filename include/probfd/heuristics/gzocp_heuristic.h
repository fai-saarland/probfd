#ifndef PROBFD_HEURISTICS_GZOCP_HEURISTIC_H
#define PROBFD_HEURISTICS_GZOCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace utils {
class RandomNumberGenerator;
class LogProxy;
} // namespace utils

namespace probfd::pdbs {
struct ProbabilityAwarePatternDatabase;
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class GZOCPHeuristic final : public FDREvaluator {
    const value_t termination_cost_;
    const std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs_;

public:
    GZOCPHeuristic(
        value_t termination_cost,
        std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs);

    ~GZOCPHeuristic() override;

    void print_statistics() const override
    {
        // TODO
    }

protected:
    value_t evaluate(const State& state) const override;
};

class GZOCPHeuristicFactory final : public TaskHeuristicFactory {
public:
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

private:
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;
    const OrderingStrategy ordering_;
    const int random_seed_;
    const utils::Verbosity verbosity_;

public:
    explicit GZOCPHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator>
            pattern_collection_generator,
        OrderingStrategy ordering,
        int random_seed,
        utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_GZOCP_HEURISTIC_H
