#ifndef PROBFD_OCCUPATION_MEASURES_CONSTRAINT_GENERATOR_H
#define PROBFD_OCCUPATION_MEASURES_CONSTRAINT_GENERATOR_H

#include "probfd/fdr_types.h"
#include "probfd/probabilistic_task.h"

#include <iosfwd>
#include <memory>

// Forward Declarations
namespace downward {
class State;
}

namespace downward::lp {
class LinearProgram;
class LPSolver;
} // namespace downward::lp

/// Namespace dedicated to occupation measure heuristic base classes
namespace probfd::occupation_measures {

class ConstraintGenerator {
public:
    virtual ~ConstraintGenerator() = default;

    /*
      Called upon initialization for the given task. Use this to add permanent
      constraints and perform other initialization.
    */
    virtual void initialize_constraints(
        const SharedProbabilisticTask& task,
        downward::lp::LinearProgram& lp) = 0;

    /*
      Called before evaluating a state. Use this to add temporary constraints
      and to set bounds on permanent constraints for this state. All temporary
      constraints are removed automatically after the evalution.
      Returns true if a dead end was detected and false otherwise.
    */
    virtual void update_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) = 0;

    /*
      Called after evaluating a state. Use this to remove to unset bounds on
      permanent constraints for this state.
    */
    virtual void reset_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) = 0;

    virtual void print_statistics(std::ostream&) {}
};

class ConstraintGeneratorFactory {
public:
    virtual ~ConstraintGeneratorFactory() = default;

    virtual std::unique_ptr<ConstraintGenerator>
    construct_constraint_generator(const SharedProbabilisticTask& task) = 0;
};

} // namespace probfd::occupation_measures

#endif // PROBFD_OCCUPATION_MEASURES_CONSTRAINT_GENERATOR_H
