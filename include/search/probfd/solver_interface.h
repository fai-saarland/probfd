#ifndef PROBFD_SOLVERS_SOLVER_INTERFACE_H
#define PROBFD_SOLVERS_SOLVER_INTERFACE_H

#include "probfd/value_type.h"

#include <iosfwd>

namespace probfd {
struct Interval;

/// An interface that describes an MDP solver.
class SolverInterface {
public:
    virtual ~SolverInterface() = default;

    virtual void print_statistics() const {}

    virtual bool found_solution() const = 0;
    virtual void solve() = 0;

protected:
    static void print_value(std::ostream& o, value_t value);

    static void print_analysis_result(Interval result);

    static void print_initial_state_value(Interval value, int spaces = 0);
};
} // namespace probfd

#endif
