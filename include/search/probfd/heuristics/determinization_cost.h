#ifndef MDPS_HEURISTICS_DETERMINIZATION_COST_H
#define MDPS_HEURISTICS_DETERMINIZATION_COST_H

#include "probfd/engine_interfaces/state_evaluator.h"

#include <memory>

class Evaluator;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {

/**
 * @brief Uses a classical heuristic on the all-outcomes-determinization to
 * estimate the expected costs to reach the goal.
 *
 * @note If the underlying classical heuristic is admissible/consistent, this
 * heuristic is also admissible/heuristic.
 */
class DeterminizationCostHeuristic : public TaskStateEvaluator {
    std::shared_ptr<Evaluator> evaluator_;

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
    explicit DeterminizationCostHeuristic(std::shared_ptr<Evaluator> heuristic);

    ~DeterminizationCostHeuristic() override;

    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const State& state) const override;
};

} // namespace heuristics
} // namespace probfd

#endif // __DETERMINIZATION_COST_H__