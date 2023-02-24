#ifndef PROBFD_HEURISTICS_LP_HEURISTIC_H
#define PROBFD_HEURISTICS_LP_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "lp/lp_solver.h"

#include "options/options.h"

#include <memory>
#include <vector>

namespace probfd {
namespace heuristics {

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
    explicit LPHeuristic(const options::Options& opts)
        : LPHeuristic(
              opts.get<std::shared_ptr<ProbabilisticTask>>("transform"),
              utils::get_log_from_options(opts),
              opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    LPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        lp::LPSolverType solver_type)
        : TaskDependentHeuristic(task, log)
        , lp_solver_(solver_type)
    {
    }

    EvaluationResult evaluate(const State& state) const override final
    {
        assert(!lp_solver_.has_temporary_constraints());

        static_cast<const Derived*>(this)->update_constraints(state);

        lp_solver_.solve();

        EvaluationResult result;

        if (lp_solver_.has_optimal_solution()) {
            result = EvaluationResult(false, lp_solver_.get_objective_value());
        } else {
            result = EvaluationResult(true, INFINITE_VALUE);
        }

        lp_solver_.clear_temporary_constraints();
        static_cast<const Derived*>(this)->reset_constraints(state);

        return result;
    }
};

} // namespace heuristics
} // namespace probfd

#endif