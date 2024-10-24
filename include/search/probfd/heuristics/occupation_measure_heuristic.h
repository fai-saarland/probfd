#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H

#include "probfd/heuristics/lp_heuristic.h"
#include "probfd/task_evaluator_factory.h"

#include <memory>

// Forward Declarations
namespace probfd::occupation_measures {
class ConstraintGenerator;
}

namespace probfd::heuristics {

/**
 * @brief Implements an occupation measure heuristic built from the specified
 * LP constraints.
 */
class OccupationMeasureHeuristic
    : public LPHeuristic<OccupationMeasureHeuristic> {
    friend class LPHeuristic<OccupationMeasureHeuristic>;

protected:
    std::shared_ptr<occupation_measures::ConstraintGenerator>
        constraint_generator_;

public:
    OccupationMeasureHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        utils::LogProxy log,
        lp::LPSolverType solver_type,
        std::shared_ptr<occupation_measures::ConstraintGenerator>
            constraint_generator);

private:
    void update_constraints(const State& state) const;
    void reset_constraints(const State& state) const;
};

class OccupationMeasureHeuristicFactory : public TaskEvaluatorFactory {
    const utils::Verbosity verbosity_;
    const lp::LPSolverType lp_solver_type_;
    const std::shared_ptr<occupation_measures::ConstraintGenerator>
        constraints_;

public:
    OccupationMeasureHeuristicFactory(
        utils::Verbosity verbosity,
        lp::LPSolverType lp_solver_type,
        const std::shared_ptr<occupation_measures::ConstraintGenerator>&
            constraints);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H
