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
 * estimate the expected costs to reach the goal.
 *
 * @note If the underlying classical heuristic is admissible/consistent, this
 * heuristic is also admissible/heuristic.
 */
class DeterminizationCostHeuristic : public GlobalStateEvaluator {
    std::shared_ptr<Heuristic> heuristic_;

public:
    /**
     * @brief Construct from options.
     *
     * @param opts - Only one option is available:
     * + heuristic - Specifies the underlying classical heuristic.
     */
    explicit DeterminizationCostHeuristic(const options::Options& opts);

    /**
     * @brief Construct from classical heuristic.
     *
     * @param heuristic - The classical heuristic.
     */
    explicit DeterminizationCostHeuristic(std::shared_ptr<Heuristic> heuristic);

    ~DeterminizationCostHeuristic() override;

    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const GlobalState& state) const override;
};

} // namespace probabilistic
