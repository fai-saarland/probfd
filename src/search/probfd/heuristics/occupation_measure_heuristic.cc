#include "probfd/heuristics/occupation_measure_heuristic.h"

#include "probfd/occupation_measures/constraint_generator.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace probfd::occupation_measures;

namespace probfd::heuristics {

OccupationMeasureHeuristic::OccupationMeasureHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    utils::LogProxy log,
    lp::LPSolverType solver_type,
    std::shared_ptr<ConstraintGenerator> constraint_generator)
    : LPHeuristic(task, std::move(log), solver_type)
    , constraint_generator_(std::move(constraint_generator))
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

OccupationMeasureHeuristicFactory::OccupationMeasureHeuristicFactory(
    utils::Verbosity verbosity,
    lp::LPSolverType lp_solver_type,
    const std::shared_ptr<ConstraintGenerator>& constraints)
    : verbosity_(verbosity)
    , lp_solver_type_(lp_solver_type)
    , constraints_(constraints)
{
}

std::unique_ptr<FDREvaluator>
OccupationMeasureHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    return std::make_unique<OccupationMeasureHeuristic>(
        std::move(task),
        std::move(task_cost_function),
        utils::get_log_for_verbosity(verbosity_),
        lp_solver_type_,
        constraints_);
}

} // namespace probfd::heuristics
