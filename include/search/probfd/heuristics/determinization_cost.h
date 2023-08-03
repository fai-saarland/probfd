#ifndef PROBFD_HEURISTICS_DETERMINIZATION_COST_H
#define PROBFD_HEURISTICS_DETERMINIZATION_COST_H

#include "probfd/evaluator.h"
#include "probfd/task_types.h"

#include <memory>

class Evaluator;

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace heuristics {

/**
 * @brief Uses a classical heuristic on the all-outcomes-determinization to
 * estimate the expected costs to reach the goal.
 *
 * @note If the underlying classical heuristic is admissible/consistent, this
 * heuristic is also admissible/heuristic.
 */
class DeterminizationCostHeuristic : public TaskEvaluator {
    std::shared_ptr<::Evaluator> evaluator_;

public:
    /**
     * @brief Construct from options.
     *
     * @param opts - Only one option is available:
     * + heuristic - Specifies the underlying classical heuristic.
     */
    explicit DeterminizationCostHeuristic(const plugins::Options& opts);

    /**
     * @brief Construct from classical heuristic.
     *
     * @param heuristic - The classical heuristic.
     */
    explicit DeterminizationCostHeuristic(
        std::shared_ptr<::Evaluator> heuristic);

    ~DeterminizationCostHeuristic() override;

    static void add_options_to_feature(plugins::Feature& feature);

    EvaluationResult evaluate(const State& state) const override;

    void print_statistics() const override;
};

} // namespace heuristics
} // namespace probfd

#endif // __DETERMINIZATION_COST_H__