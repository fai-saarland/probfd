#ifndef PROBFD_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H
#define PROBFD_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/probabilistic_task.h"
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
 * estimate the expected costs to reach the goal.
 *
 * @note If the underlying classical heuristic is admissible/consistent, this
 * heuristic is also admissible/heuristic.
 */
class DeterminizationCostHeuristic final : public FDREvaluator {
    const std::shared_ptr<downward::Evaluator> evaluator_;

public:
    /**
     * @brief Construct from a classical heuristic.
     */
    explicit DeterminizationCostHeuristic(
        std::shared_ptr<downward::Evaluator> heuristic);

    ~DeterminizationCostHeuristic() override;

    [[nodiscard]]
    value_t evaluate(const downward::State& state) const override;

    void print_statistics() const override;
};

class DeterminizationCostHeuristicFactory final : public TaskHeuristicFactory {
    const std::shared_ptr<downward::Evaluator> evaluator_;

public:
    /**
     * @brief Construct from determinization-based heuristic.
     */
    explicit DeterminizationCostHeuristicFactory(
        std::shared_ptr<downward::Evaluator> evaluator);

    std::unique_ptr<FDREvaluator> create_heuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif