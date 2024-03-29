#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURES_OCCUPATION_MEASURE_HEURISTIC_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURES_OCCUPATION_MEASURE_HEURISTIC_H

#include "probfd/heuristics/lp_heuristic.h"

#include <memory>
#include <vector>

namespace plugins {
class Options;
}

namespace probfd {
namespace heuristics {

/// Namespace dedicated to occupation measure heuristics
namespace occupation_measures {

class ConstraintGenerator;

/**
 * @brief Implements an occupation measure heuristic built from the specified
 * LP constraints.
 */
class OccupationMeasureHeuristic
    : public LPHeuristic<OccupationMeasureHeuristic> {
    friend class LPHeuristic<OccupationMeasureHeuristic>;

protected:
    std::shared_ptr<ConstraintGenerator> constraint_generator_;

public:
    explicit OccupationMeasureHeuristic(const plugins::Options& opts);

    OccupationMeasureHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        utils::LogProxy log,
        lp::LPSolverType solver_type,
        std::shared_ptr<ConstraintGenerator> constraint_generator);

private:
    void update_constraints(const State& state) const;
    void reset_constraints(const State& state) const;
};

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd

#endif