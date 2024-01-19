#ifndef PROBFD_HEURISTICS_DETERMINIZATION_COST_H
#define PROBFD_HEURISTICS_DETERMINIZATION_COST_H

#include "probfd/evaluator.h"
#include "probfd/fdr_types.h"
#include "probfd/value_type.h"

#include <memory>

// Forward Declarations
class State;
class Evaluator;

namespace probfd::heuristics {

/**
 * @brief Uses a classical heuristic on the all-outcomes-determinization to
 * estimate the expected costs to reach the goal.
 *
 * @note If the underlying classical heuristic is admissible/consistent, this
 * heuristic is also admissible/heuristic.
 */
class DeterminizationCostHeuristic : public FDREvaluator {
    const std::shared_ptr<::Evaluator> evaluator_;

public:
    /**
     * @brief Construct from a classical heuristic.
     */
    explicit DeterminizationCostHeuristic(
        std::shared_ptr<::Evaluator> heuristic);

    ~DeterminizationCostHeuristic() override;

    [[nodiscard]]
    value_t evaluate(const State& state) const override;

    void print_statistics() const override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_DETERMINIZATION_COST_H