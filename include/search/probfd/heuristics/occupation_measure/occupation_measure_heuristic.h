#ifndef MDPS_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H
#define MDPS_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H

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
 * @brief Implements the projection occupation measure heuristic
 * \cite trevizan:etal:icaps-17 .
 */
class ProjectionOccupationMeasureHeuristic : public TaskDependentHeuristic {
    mutable lp::LPSolver lp_solver_;
    std::vector<int> offset_;
    bool is_maxprob_;

public:
    /**
     * @brief Construct from options.
     *
     * @param opts - Available options:
     * + \em lpsolver - The LP solver that should be used internally. One of
     * \em CLP, \em CPLEX, \em GUROBI or \em SOPLEX.
     */
    explicit ProjectionOccupationMeasureHeuristic(const options::Options& opts);

    static void generate_hpom_lp(
        const ProbabilisticTaskProxy& task_proxy,
        lp::LPSolver& lp_solver,
        named_vector::NamedVector<lp::LPVariable>& vars,
        named_vector::NamedVector<lp::LPConstraint>& constraints,
        std::vector<int>& offsets,
        bool maxprob);

    static void add_options_to_parser(options::OptionParser& parser);

    virtual EvaluationResult evaluate(const State& state) const override;
};

} // namespace occupation_measure_heuristic
} // namespace heuristics
} // namespace probfd

#endif // __OCCUPATION_MEASURE_HEURISTIC_H__