#ifndef PROBFD_HEURISTICS_LP_HEURISTIC_H
#define PROBFD_HEURISTICS_LP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "downward/lp/lp_solver.h"

#include <memory>
#include <vector>

namespace probfd::heuristics {

/**
 * @brief Base class for heuristics based on linear programming.
 *
 * This class uses CRTP and requires the methods:
 *
 * ```
 * void update_constraints(const State& state) const;
 * void reset_constraints(const State& state) const;
 * ```
 */
template <typename Derived>
class LPHeuristic : public TaskDependentHeuristic {
protected:
    mutable lp::LPSolver lp_solver_;

public:
    LPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        lp::LPSolverType solver_type)
        : TaskDependentHeuristic(std::move(task), std::move(log))
        , lp_solver_(solver_type)
    {
    }

    value_t evaluate(const State& state) const final
    {
        assert(!lp_solver_.has_temporary_constraints());

        static_cast<const Derived*>(this)->update_constraints(state);

        lp_solver_.solve();

        const value_t result = lp_solver_.has_optimal_solution()
                                   ? lp_solver_.get_objective_value()
                                   : INFINITE_VALUE;

        lp_solver_.clear_temporary_constraints();
        static_cast<const Derived*>(this)->reset_constraints(state);

        return result;
    }
};

} // namespace probfd::heuristics

#endif