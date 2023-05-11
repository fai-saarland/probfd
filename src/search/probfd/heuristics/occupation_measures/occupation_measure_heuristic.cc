#include "probfd/heuristics/occupation_measures/occupation_measure_heuristic.h"

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "utils/markup.h"

#include "option_parser.h"
#include "plugin.h"

#include <cmath>

using namespace std;
using namespace options;

namespace probfd {
namespace heuristics {
namespace occupation_measures {

OccupationMeasureHeuristic::OccupationMeasureHeuristic(const Options& opts)
    : OccupationMeasureHeuristic(
          opts.get<std::shared_ptr<ProbabilisticTask>>("transform"),
          opts.get<lp::LPSolverType>("lpsolver"),
          opts.get<shared_ptr<ConstraintGenerator>>("constraint_generator"))
{
}

OccupationMeasureHeuristic::OccupationMeasureHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    lp::LPSolverType solver_type,
    std::shared_ptr<ConstraintGenerator> constraint_generator)
    : LPHeuristic(task, solver_type)
    , constraint_generator_(constraint_generator)
{
    lp::LinearProgram lp(
        lp::LPObjectiveSense::MINIMIZE,
        named_vector::NamedVector<lp::LPVariable>(),
        named_vector::NamedVector<lp::LPConstraint>(),
        lp_solver_.get_infinity());

    constraint_generator_->initialize_constraints(task, lp);

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