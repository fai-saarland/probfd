#ifndef PROBFD_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H
#define PROBFD_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"
#include "probfd/value_type.h"

#include <memory>

// Forward Declarations
namespace downward {
class State;
class Evaluator;
}

namespace probfd::heuristics {

/**
 * @brief Uses a classical heuristic on the all-outcomes-determinization to
 * prune dead-ends.
 *
 * @note If the underlying classical heuristic is safe, this heuristic is also
 * safe.
 */
class DeadEndPruningHeuristic final : public FDREvaluator {
    const std::shared_ptr<downward::Evaluator> pruning_function_;
    const value_t dead_end_value_;

public:
    /**
     * @brief Construct with the given classical heuristic.
     *
     * @param pruning_function - The classical heuristic.
     * @param dead_end_value - State estimate returned for states that are
     * dead-ends according to the classical heuristic.
     */
    DeadEndPruningHeuristic(
        std::shared_ptr<downward::Evaluator> pruning_function,
        value_t dead_end_value);

    [[nodiscard]]
    value_t evaluate(const downward::State& state) const override;

    void print_statistics() const override;
};

class DeadEndPruningHeuristicFactory final : public TaskHeuristicFactory {
    const std::shared_ptr<downward::Evaluator> evaluator_;

public:
    explicit DeadEndPruningHeuristicFactory(
        std::shared_ptr<downward::Evaluator> evaluator);

    std::unique_ptr<FDREvaluator> create_heuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_DEAD_END_PRUNING_HEURISTIC_H