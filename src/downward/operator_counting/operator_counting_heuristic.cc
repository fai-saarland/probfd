#include "downward/operator_counting/operator_counting_heuristic.h"

#include "downward/operator_cost_function.h"
#include "downward/operator_cost_function_fwd.h"
#include "downward/operator_counting/constraint_generator.h"

#include "downward/utils/markup.h"

#include "downward/task_transformation.h"

#include <cmath>

using namespace std;

namespace downward::operator_counting {
OperatorCountingHeuristic::OperatorCountingHeuristic(
    const vector<shared_ptr<ConstraintGenerator>>& constraint_generators,
    bool use_integer_operator_counts,
    lp::LPSolverType lpsolver,
    SharedAbstractTask original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
    , constraint_generators(constraint_generators)
    , lp_solver(lpsolver)
{
    const auto& operators =
        get_operators(transformed_task);
    const auto& cost_function =
        get_cost_function(transformed_task);

    lp_solver.set_mip_gap(0);
    named_vector::NamedVector<lp::LPVariable> variables;
    double infinity = lp_solver.get_infinity();
    for (OperatorProxy op : operators) {
        int op_cost = cost_function.get_operator_cost(op.get_id());
        variables.push_back(
            lp::LPVariable(0, infinity, op_cost, use_integer_operator_counts));
#ifndef NDEBUG
        variables.set_name(op.get_id(), op.get_name());
#endif
    }
    lp::LinearProgram lp(
        lp::LPObjectiveSense::MINIMIZE,
        std::move(variables),
        {},
        infinity);
    for (const auto& generator : constraint_generators) {
        generator->initialize_constraints(transformed_task, lp);
    }
    lp_solver.load_problem(lp);
}

OperatorCountingHeuristic::OperatorCountingHeuristic(
    const std::vector<std::shared_ptr<ConstraintGenerator>>&
        constraint_generators,
    bool use_integer_operator_counts,
    lp::LPSolverType lpsolver,
    SharedAbstractTask original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : OperatorCountingHeuristic(
          constraint_generators,
          use_integer_operator_counts,
          lpsolver,
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

int OperatorCountingHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    assert(!lp_solver.has_temporary_constraints());
    for (const auto& generator : constraint_generators) {
        bool dead_end = generator->update_constraints(state, lp_solver);
        if (dead_end) {
            lp_solver.clear_temporary_constraints();
            return DEAD_END;
        }
    }
    int result;
    lp_solver.solve();
    if (lp_solver.has_optimal_solution()) {
        double epsilon = 0.01;
        double objective_value = lp_solver.get_objective_value();
        result = static_cast<int>(ceil(objective_value - epsilon));
    } else {
        result = DEAD_END;
    }
    lp_solver.clear_temporary_constraints();
    return result;
}

} // namespace downward::operator_counting
