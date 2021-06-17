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
 * @brief Implements the projection occupation measure heuristic
 * \cite trevizan:etal:icaps-17 .
 */
class ProjectionOccupationMeasureHeuristic : public GlobalStateEvaluator {
public:
    /**
     * @brief Construct from options.
     * 
     * @param opts - Available options:
     * + \em lpsolver - The LP solver that should be used internally. One of 
     * \em CLP, \em CPLEX, \em GUROBI or \em SOPLEX.
     */
    explicit ProjectionOccupationMeasureHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    static void generate_hpom_lp(
        lp::LPSolver& lp_solver,
        std::vector<lp::LPVariable>& vars,
        std::vector<lp::LPConstraint>& constraints,
        std::vector<int>& offsets);

    static void generate_hpom_lp_expcost(
        lp::LPSolver& lp_solver,
        std::vector<lp::LPVariable>& vars,
        std::vector<lp::LPConstraint>& constraints,
        std::vector<int>& offsets);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    lp::LPSolver lp_solver_;
    std::vector<int> offset_;
};

} // namespace occupation_measure_heuristic
} // namespace probabilistic
