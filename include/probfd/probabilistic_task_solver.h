#ifndef PROBFD_PROBABILISTIC_TASK_SOLVER_H
#define PROBFD_PROBABILISTIC_TASK_SOLVER_H

#include "probfd/probabilistic_task.h"

#include "downward/utils/timer.h"

#include <memory>

// Forward Declarations
namespace probfd {
struct Interval;
} // namespace probfd

namespace probfd {

/// An interface that describes a planning problem solver.
class ProbabilisticTaskSolver {
public:
    virtual ~ProbabilisticTaskSolver() = default;

    virtual void print_statistics() const {}

    virtual bool solve(
        const SharedProbabilisticTask& task,
        downward::utils::FSeconds max_time) = 0;
};

extern void print_analysis_result(Interval result);

extern void print_initial_state_value(Interval value, int spaces = 0);

} // namespace probfd

#endif // PROBFD_PROBABILISTIC_TASK_SOLVER_H
