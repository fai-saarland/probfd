#ifndef SOLVER_INTERFACE_H
#define SOLVER_INTERFACE_H

#include "probfd/value_type.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <iosfwd>
#include <memory>

// Forward Declarations
namespace probfd {
class ProbabilisticTask;

struct Interval;
} // namespace probfd

namespace probfd {

extern void print_analysis_result(Interval result);

extern void print_initial_state_value(Interval value, int spaces = 0);

/// An interface that describes an MDP solver.
class SolverInterface {
public:
    virtual ~SolverInterface() = default;

    virtual void print_statistics() const {}

    virtual bool solve(double max_time) = 0;
};

class TaskSolverFactory {
public:
    virtual ~TaskSolverFactory() = default;

    virtual std::unique_ptr<SolverInterface>
    create(const std::shared_ptr<ProbabilisticTask>& task) = 0;
};

} // namespace probfd

#endif // SOLVER_INTERFACE_H
