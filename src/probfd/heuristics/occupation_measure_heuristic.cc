#include "probfd/heuristics/occupation_measure_heuristic.h"

#include "downward/operator_cost_function.h"
#include "probfd/occupation_measures/constraint_generator.h"

#include "probfd/probabilistic_task.h"

#include "probfd/probabilistic_operator_space.h"

using namespace std;
using namespace downward;
using namespace probfd::occupation_measures;

namespace probfd::heuristics {

OccupationMeasureHeuristic::OccupationMeasureHeuristic(
    SharedProbabilisticTask task,
    lp::LPSolverType solver_type,
    std::shared_ptr<ConstraintGenerator> constraint_generator)
    : LPHeuristic(std::move(task), solver_type)
    , constraint_generator_(std::move(constraint_generator))
{
    lp::LinearProgram lp(
        lp::LPObjectiveSense::MINIMIZE,
        named_vector::NamedVector<lp::LPVariable>(),
        named_vector::NamedVector<lp::LPConstraint>(),
        lp_solver_.get_infinity());

    const auto& operators = get_operators(task_);
    const auto& cost_function = get_cost_function(task_);

    auto& lp_variables = lp.get_variables();

    for (const auto& op : operators) {
        lp_variables.emplace_back(
            0.0,
            lp.get_infinity(),
            cost_function.get_operator_cost(op.get_id()));
    }

    constraint_generator_->initialize_constraints(task_, lp);

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
    lp::LPSolverType lp_solver_type,
    const std::shared_ptr<ConstraintGeneratorFactory>&
        constraint_generator_factory)
    : lp_solver_type_(lp_solver_type)
    , constraint_generator_factory_(constraint_generator_factory)
{
}

std::unique_ptr<FDRHeuristic> OccupationMeasureHeuristicFactory::create_object(
    const SharedProbabilisticTask& task)
{
    auto constraints =
        constraint_generator_factory_->construct_constraint_generator(task);

    return std::make_unique<OccupationMeasureHeuristic>(
        task,
        lp_solver_type_,
        std::move(constraints));
}

} // namespace probfd::heuristics
