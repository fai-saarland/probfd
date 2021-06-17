#pragma once

#include "../state_evaluator.h"

#include <memory>

class Heuristic;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

/**
 * @brief Uses a classical heuristic on the all-outcomes-determinization to
 * prune dead-ends.
 * 
 * @note If the underlying classical heuristic is safe, this heuristic is also 
 * safe.
 */
class DeadEndPruningHeuristic : public GlobalStateEvaluator {
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
        value_type::value_t default_value,
        value_type::value_t dead_end_value,
        std::shared_ptr<Heuristic> pruning_function);

    /**
     * @brief Construct from options.
     * 
     * @param opts - Available options:
     * + \em pessimistic - If true, returns g_analysis_objective_max() for 
     * states that the deterministic heuristic does not recognize as dead-ends.
     * Otherwise return g_analysis_objective_min() for such states.
     * + \em heuristic - The classical heuristic.
     */
    explicit DeadEndPruningHeuristic(const options::Options& opts);

    static void add_options_to_parser(options::OptionParser& parser);

    virtual void print_statistics() const override;

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    const value_type::value_t default_value_;
    const value_type::value_t dead_end_value_;
    std::shared_ptr<Heuristic> pruning_function_;
};

} // namespace probabilistic
