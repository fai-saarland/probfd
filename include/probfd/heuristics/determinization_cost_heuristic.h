#ifndef PROBFD_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H
#define PROBFD_HEURISTICS_DETERMINIZATION_COST_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/probabilistic_task.h"
#include "probfd/task_heuristic_factory_category.h"
#include "probfd/value_type.h"

#include "downward/task_dependent_factory_fwd.h"

#include <memory>

// Forward Declarations
namespace downward {
class State;
class Evaluator;
} // namespace downward

namespace probfd::heuristics {

/**
 * @brief Uses a classical heuristic on the all-outcomes-determinization to
 * estimate the expected costs to reach the goal.
 *
 * @note If the underlying classical heuristic is admissible/consistent, this
 * heuristic is also admissible/heuristic.
 */
class DeterminizationCostHeuristic final : public FDRHeuristic {
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
    const std::shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>
        evaluator_factory_;

public:
    /**
     * @brief Construct from determinization-based heuristic.
     */
    explicit DeterminizationCostHeuristicFactory(
        std::shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>
            evaluator_factory);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif