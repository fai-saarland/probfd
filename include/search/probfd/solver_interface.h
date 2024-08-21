#ifndef PROBFD_SOLVERS_SOLVER_INTERFACE_H
#define PROBFD_SOLVERS_SOLVER_INTERFACE_H

#include "probfd/value_type.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <iosfwd>
#include <memory>

// Forward Declarations
namespace probfd {
struct Interval;
template <typename, typename>
class Policy;
} // namespace probfd

namespace probfd {

extern void print_value(std::ostream& o, value_t value);

extern void print_analysis_result(Interval result);

extern void print_initial_state_value(Interval value, int spaces = 0);

/// An interface that describes an MDP solver.
class SolverInterface {
public:
    virtual ~SolverInterface() = default;

    virtual void print_statistics() const {}

    virtual bool solve() = 0;
};

} // namespace probfd

#endif
