#include "probfd/heuristics/occupation_measures/occupation_measure_heuristic.h"

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "downward/utils/markup.h"

#include "downward/plugins/plugin.h"

#include <cmath>

using namespace std;

namespace probfd {
namespace heuristics {
namespace occupation_measures {

OccupationMeasureHeuristic::OccupationMeasureHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    utils::LogProxy log,
    lp::LPSolverType solver_type,
    std::shared_ptr<ConstraintGenerator> constraint_generator)
    : LPHeuristic(task, log, solver_type)
    , constraint_generator_(constraint_generator)
{
    lp::LinearProgram lp(
        lp::LPObjectiveSense::MINIMIZE,
        named_vector::NamedVector<lp::LPVariable>(),
        named_vector::NamedVector<lp::LPConstraint>(),
        lp_solver_.get_infinity());

    constraint_generator_->initialize_constraints(task, task_cost_function, lp);

    lp_solver_.load_problem(lp);
}

void OccupationMeasureHeuristic::update_constraints(const State& state) const
{
    constraint_generator_->update_constraints(state, lp_solver_);
}

void OccupationMeasureHeuristic::reset_constraints(const State& state) const
{
    constraint_generator_->reset_constraints(state, lp_solver_);
}

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd