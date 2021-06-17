#pragma once

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
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    lp::LPSolver lp_solver_;
    std::vector<int> constraint_offsets_;

private:
    void load_expcost_lp();
    void load_maxprob_lp();
};

} // namespace occupation_measure_heuristic
} // namespace probabilistic
