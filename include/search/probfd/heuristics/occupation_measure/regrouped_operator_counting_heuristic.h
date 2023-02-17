#ifndef PROBFD_HEURISTICS_REGROUPED_OPERATOR_COUNTING_HEURISTIC_H
#define PROBFD_HEURISTICS_REGROUPED_OPERATOR_COUNTING_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "lp/lp_solver.h"

#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {
/// Namespace dedicated to occupation measure heuristics
namespace occupation_measure_heuristic {

/**
 * @brief Implements the regrouped operator counting heuristic
 * \cite trevizan:etal:icaps-17 .
 */
class RegroupedOperatorCountingHeuristic : public TaskDependentHeuristic {
    mutable lp::LPSolver lp_solver_;
    std::vector<std::size_t> ncc_offsets_;
    const bool is_maxprob;

    mutable std::vector<int> reset_indices_;

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

    EvaluationResult evaluate(const State& state) const override;
};

} // namespace occupation_measure_heuristic
} // namespace heuristics
} // namespace probfd

#endif // __REGROUPED_OPERATOR_COUNTING_HEURISTIC_H__