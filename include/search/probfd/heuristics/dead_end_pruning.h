#ifndef PROBFD_HEURISTICS_DEAD_END_PRUNING_H
#define PROBFD_HEURISTICS_DEAD_END_PRUNING_H

#include "probfd/evaluator.h"
#include "probfd/task_evaluator_factory.h"
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
 * prune dead-ends.
 *
 * @note If the underlying classical heuristic is safe, this heuristic is also
 * safe.
 */
class DeadEndPruningHeuristic : public TaskEvaluator {
    const std::shared_ptr<::Evaluator> pruning_function_;
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
        std::shared_ptr<::Evaluator> pruning_function,
        value_t dead_end_value);

    EvaluationResult evaluate(const State& state) const override;

    void print_statistics() const override;
};

} // namespace heuristics
} // namespace probfd

#endif // __DEAD_END_PRUNING_H__