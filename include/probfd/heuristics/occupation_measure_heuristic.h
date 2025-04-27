#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H

#include "probfd/heuristics/lp_heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include <memory>

// Forward Declarations
namespace probfd::occupation_measures {
class ConstraintGenerator;
class ConstraintGeneratorFactory;
} // namespace probfd::occupation_measures

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
        SharedProbabilisticTask task,
        downward::utils::LogProxy log,
        downward::lp::LPSolverType solver_type,
        std::shared_ptr<occupation_measures::ConstraintGenerator>
            constraint_generator);

private:
    void update_constraints(const downward::State& state) const;
    void reset_constraints(const downward::State& state) const;
};

class OccupationMeasureHeuristicFactory : public TaskHeuristicFactory {
    const downward::utils::Verbosity verbosity_;
    const downward::lp::LPSolverType lp_solver_type_;
    const std::shared_ptr<occupation_measures::ConstraintGeneratorFactory>
        constraint_generator_factory_;

public:
    OccupationMeasureHeuristicFactory(
        downward::utils::Verbosity verbosity,
        downward::lp::LPSolverType lp_solver_type,
        const std::shared_ptr<occupation_measures::ConstraintGeneratorFactory>&
            constraint_generator_factory);

    std::unique_ptr<FDREvaluator>
    create_heuristic(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H
