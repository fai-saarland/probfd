#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H

#include "probfd/heuristics/lp_heuristic.h"

#include <memory>

// Forward Declarations
namespace plugins {
class Options;
}

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

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTIC_H
