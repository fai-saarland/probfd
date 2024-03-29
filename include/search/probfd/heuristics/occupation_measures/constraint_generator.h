#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTICS_CONSTRAINT_GENERATOR_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTICS_CONSTRAINT_GENERATOR_H

#include "probfd/fdr_types.h"

#include <memory>
#include <vector>

class State;

namespace lp {
class LinearProgram;
class LPSolver;
} // namespace lp

namespace probfd {
class ProbabilisticTask;

namespace heuristics {
namespace occupation_measures {

class ConstraintGenerator {
public:
    virtual ~ConstraintGenerator() = default;

    /*
      Called upon initialization for the given task. Use this to add permanent
      constraints and perform other initialization.
    */
    virtual void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        lp::LinearProgram& lp) = 0;

    /*
      Called before evaluating a state. Use this to add temporary constraints
      and to set bounds on permanent constraints for this state. All temporary
      constraints are removed automatically after the evalution.
      Returns true if a dead end was detected and false otherwise.
    */
    virtual void
    update_constraints(const State& state, lp::LPSolver& solver) = 0;

    /*
      Called after evaluating a state. Use this to remove to unset bounds on
      permanent constraints for this state.
    */
    virtual void
    reset_constraints(const State& state, lp::LPSolver& solver) = 0;

    virtual void print_statistics(std::ostream&) {}
};
} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd

#endif