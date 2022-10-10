#ifndef MDPS_HEURISTICS_REGROUPED_OPERATOR_COUNTING_HEURISTIC_H
#define MDPS_HEURISTICS_REGROUPED_OPERATOR_COUNTING_HEURISTIC_H

#include "../../../lp/lp_solver.h"
#include "../../state_evaluator.h"

#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

/// Namespace dedicated to occupation measure heuristics
namespace occupation_measure_heuristic {

/**
 * @brief Implements the regrouped operator counting heuristic
 * \cite trevizan:etal:icaps-17 .
 */
class RegroupedOperatorCountingHeuristic : public GlobalStateEvaluator {
public:
    /**
     * @brief Construct from options.
     *
     * @param opts - Available options:
     * + \em lpsolver - The LP solver that should be used internally. One of
     * \em CLP, \em CPLEX, \em GUROBI or \em SOPLEX.
     */
    explicit RegroupedOperatorCountingHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) const override;

    mutable lp::LPSolver lp_solver_;
    std::vector<std::size_t> ncc_offsets_;
    const bool is_maxprob;

    mutable std::vector<int> reset_indices_;
};

} // namespace occupation_measure_heuristic
} // namespace probabilistic

#endif // __REGROUPED_OPERATOR_COUNTING_HEURISTIC_H__