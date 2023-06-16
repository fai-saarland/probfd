#ifndef PROBFD_HEURISTICS_DEAD_END_PRUNING_H
#define PROBFD_HEURISTICS_DEAD_END_PRUNING_H

#include "probfd/engine_interfaces/evaluator.h"

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
    const value_t default_value_;
    const value_t dead_end_value_;
    std::shared_ptr<::Evaluator> pruning_function_;

public:
    /**
     * @brief Construct with the given clasical heuristic.
     *
     * @param default_value - State estimate returned for states that are not
     * dead-ends according to the classical heuristic.
     * @param dead_end_value - State estimate returned for states that are
     * dead-ends according to the classical heuristic.
     * @param pruning_function - The classical heuristic.
     */
    DeadEndPruningHeuristic(
        value_t default_value,
        value_t dead_end_value,
        std::shared_ptr<::Evaluator> pruning_function);

    /**
     * @brief Construct from options.
     *
     * @param opts - Available options:
     * + \em pessimistic - If true, returns g_analysis_objective_max() for
     * states that the deterministic heuristic does not recognize as dead-ends.
     * Otherwise return g_analysis_objective_min() for such states.
     * + \em heuristic - The classical heuristic.
     */
    explicit DeadEndPruningHeuristic(const plugins::Options& opts);

    static void add_options_to_feature(plugins::Feature& feature);

    virtual void print_statistics() const override;

    virtual EvaluationResult evaluate(const State& state) const override;
};

} // namespace heuristics
} // namespace probfd

#endif // __DEAD_END_PRUNING_H__