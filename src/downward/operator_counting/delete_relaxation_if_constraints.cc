#include "downward/operator_counting/delete_relaxation_if_constraints.h"

#include "downward/abstract_task.h"
#include "downward/classical_operator_space.h"
#include "downward/goal_fact_list.h"
#include "downward/state.h"

#include "downward/lp/lp_solver.h"

#include "downward/utils/markup.h"

#include <cassert>

using namespace std;

namespace downward::operator_counting {
static void add_lp_variables(
    int count,
    LPVariables& variables,
    vector<int>& indices,
    double lower,
    double upper,
    double objective,
    bool is_integer)
{
    for (int i = 0; i < count; ++i) {
        indices.push_back(variables.size());
        variables.emplace_back(lower, upper, objective, is_integer);
    }
}

DeleteRelaxationIFConstraints::DeleteRelaxationIFConstraints(
    bool use_time_vars,
    bool use_integer_vars)
    : use_time_vars(use_time_vars)
    , use_integer_vars(use_integer_vars)
{
}

int DeleteRelaxationIFConstraints::get_var_op_used(
    const OperatorProxy& op) const
{
    return lp_var_id_op_used[op.get_id()];
}

int DeleteRelaxationIFConstraints::get_var_fact_reached(FactPair f) const
{
    return lp_var_id_fact_reached[f.var][f.value];
}

int DeleteRelaxationIFConstraints::get_var_first_achiever(
    const OperatorProxy& op,
    FactPair f) const
{
    return lp_var_id_first_achiever[op.get_id()][f.var][f.value];
}

int DeleteRelaxationIFConstraints::get_var_op_time(
    const OperatorProxy& op) const
{
    return lp_var_id_op_time[op.get_id()];
}

int DeleteRelaxationIFConstraints::get_var_fact_time(FactPair f) const
{
    return lp_var_id_fact_time[f.var][f.value];
}

int DeleteRelaxationIFConstraints::get_constraint_id(FactPair f) const
{
    return constraint_ids[f.var][f.value];
}

void DeleteRelaxationIFConstraints::create_auxiliary_variables(
    const VariableSpace& variables,
    const ClassicalOperatorSpace& operators,
    LPVariables& lp_variables)
{
    int num_ops = operators.size();
    int num_vars = variables.size();

    // op_used
    add_lp_variables(
        num_ops,
        lp_variables,
        lp_var_id_op_used,
        0,
        1,
        0,
        use_integer_vars);

    // fact_reached
    lp_var_id_fact_reached.resize(num_vars);
    for (VariableProxy var : variables) {
        add_lp_variables(
            var.get_domain_size(),
            lp_variables,
            lp_var_id_fact_reached[var.get_id()],
            0,
            1,
            0,
            use_integer_vars);
    }

    // first_achiever
    lp_var_id_first_achiever.resize(num_ops);
    for (OperatorProxy op : operators) {
        lp_var_id_first_achiever[op.get_id()].resize(num_vars);
        for (VariableProxy var : variables) {
            add_lp_variables(
                var.get_domain_size(),
                lp_variables,
                lp_var_id_first_achiever[op.get_id()][var.get_id()],
                0,
                1,
                0,
                use_integer_vars);
        }
    }

    if (use_time_vars) {
        // op_time
        add_lp_variables(
            num_ops,
            lp_variables,
            lp_var_id_op_time,
            0,
            num_ops,
            0,
            use_integer_vars);

        // fact_time
        lp_var_id_fact_time.resize(num_vars);
        for (VariableProxy var : variables) {
            add_lp_variables(
                var.get_domain_size(),
                lp_variables,
                lp_var_id_fact_time[var.get_id()],
                0,
                num_ops,
                0,
                use_integer_vars);
        }
    }
}

void DeleteRelaxationIFConstraints::create_constraints(
    const VariableSpace& variables,
    const ClassicalOperatorSpace& operators,
    const GoalFactList& goals,
    lp::LinearProgram& lp)
{
    LPVariables& lp_variables = lp.get_variables();
    LPConstraints& lp_constraints = lp.get_constraints();
    double infinity = lp.get_infinity();

    /*
      All goal facts must be reached (handled in variable bound instead of
      constraint).
          R_f = 1 for all goal facts f.
    */
    for (FactPair goal : goals) {
        lp_variables[get_var_fact_reached(goal)].lower_bound = 1;
    }

    /*
      A fact is reached if it has a first achiever or is true in the
      current state.
          sum_{o \in achievers(f)} F_{o,f} - R_f >= [s |= f] for each fact f.
    */
    constraint_ids.resize(variables.size());
    for (VariableProxy var : variables) {
        int var_id = var.get_id();
        constraint_ids[var_id].resize(var.get_domain_size());
        for (int value = 0; value < var.get_domain_size(); ++value) {
            constraint_ids[var_id][value] = lp_constraints.size();
            lp_constraints.emplace_back(0, infinity);
            /* We add "- R_f" here, collect the achiever below and adapt
               the lower bound in each iteration, i.e., in
               update_constraints. */
            lp_constraints.back().insert(
                get_var_fact_reached(FactPair(var_id, value)),
                -1);
        }
    }

    for (OperatorProxy op : operators) {
        for (auto eff : op.get_effects()) {
            FactPair f = eff.get_fact();
            lp::LPConstraint& constraint = lp_constraints[get_constraint_id(f)];
            constraint.insert(get_var_first_achiever(op, f), 1);
        }
    }

    /*
      If an operator is a first achiever, it must be used.
          U_o >= F_{o,f} for each operator o and each of its effects f.
    */
    for (OperatorProxy op : operators) {
        for (auto eff : op.get_effects()) {
            FactPair f = eff.get_fact();
            lp::LPConstraint constraint(0, infinity);
            constraint.insert(get_var_op_used(op), 1);
            constraint.insert(get_var_first_achiever(op, f), -1);
            lp_constraints.push_back(constraint);
        }
    }

    /*
      If an operator is used, its preconditions must be reached.
          R_f >= U_o for each operator o and each of its preconditions f.
    */
    for (OperatorProxy op : operators) {
        for (FactPair f : op.get_preconditions()) {
            lp::LPConstraint constraint(0, infinity);
            constraint.insert(get_var_fact_reached(f), 1);
            constraint.insert(get_var_op_used(op), -1);
            lp_constraints.push_back(constraint);
        }
    }

    if (use_time_vars) {
        /*
          Preconditions must be reached before the operator is used.
              T_f <= T_o for each operator o and each of its preconditions f.
        */
        for (OperatorProxy op : operators) {
            for (FactPair f : op.get_preconditions()) {
                lp::LPConstraint constraint(0, infinity);
                constraint.insert(get_var_op_time(op), 1);
                constraint.insert(get_var_fact_time(f), -1);
                lp_constraints.push_back(constraint);
            }
        }

        /*
          If an operator is a first achiever, its effects are reached in
          the time step following its use.
              T_o + 1 <= T_f + M(1 - F_{o,f})
              for each operator o and each of its effects f.
          We rewrite this as
              1 - M <= T_f - T_o - M*F_{o,f} <= infty
        */
        int M = operators.size() + 1;
        for (OperatorProxy op : operators) {
            for (auto eff : op.get_effects()) {
                FactPair f = eff.get_fact();
                lp::LPConstraint constraint(1 - M, infinity);
                constraint.insert(get_var_fact_time(f), 1);
                constraint.insert(get_var_op_time(op), -1);
                constraint.insert(get_var_first_achiever(op, f), -M);
                lp_constraints.push_back(constraint);
            }
        }
    }

    /*
      If an operator is used, it must occur at least once.
          U_o <= C_o for each operator o.
    */
    for (OperatorProxy op : operators) {
        lp::LPConstraint constraint(0, infinity);
        constraint.insert(op.get_id(), 1);
        constraint.insert(get_var_op_used(op), -1);
        lp_constraints.push_back(constraint);
    }
}

void DeleteRelaxationIFConstraints::initialize_constraints(
    const SharedAbstractTask& task,
    lp::LinearProgram& lp)
{
    const auto& [variables, operators] =
        slice_shared<VariableSpace, ClassicalOperatorSpace>(task);

    create_auxiliary_variables(*variables, *operators, lp.get_variables());
    create_constraints(
        *variables,
        *operators,
        get_goal(task),
        lp);
}

bool DeleteRelaxationIFConstraints::update_constraints(
    const State& state,
    lp::LPSolver& lp_solver)
{
    // Unset old bounds.
    for (FactPair f : last_state) {
        lp_solver.set_constraint_lower_bound(get_constraint_id(f), 0);
    }
    last_state.clear();
    // Set new bounds.
    for (FactPair f : state | as_fact_pair_set) {
        lp_solver.set_constraint_lower_bound(get_constraint_id(f), -1);
        last_state.push_back(f);
    }
    return false;
}

} // namespace downward::operator_counting
