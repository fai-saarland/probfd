#include "downward/operator_counting/state_equation_constraints.h"

#include "downward/lp/lp_solver.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;

namespace downward::operator_counting {
StateEquationConstraints::StateEquationConstraints(utils::Verbosity verbosity)
    : log(utils::get_log_for_verbosity(verbosity))
{
}

static void add_indices_to_constraint(
    lp::LPConstraint& constraint,
    const set<int>& indices,
    double coefficient)
{
    for (int index : indices) { constraint.insert(index, coefficient); }
}

void StateEquationConstraints::build_propositions(
    const VariableSpace& variables,
    const ClassicalOperatorSpace& operators)
{
    propositions.reserve(variables.size());
    for (VariableProxy var : variables) {
        propositions.push_back(vector<Proposition>(var.get_domain_size()));
    }

    for (size_t op_id = 0; op_id < operators.size(); ++op_id) {
        const OperatorProxy& op = operators[op_id];
        vector<int> precondition(variables.size(), -1);
        for (const auto [var, value] : op.get_preconditions()) {
            precondition[var] = value;
        }
        for (auto effect_proxy : op.get_effects()) {
            FactPair effect = effect_proxy.get_fact();
            int var = effect.var;
            int pre = precondition[var];
            int post = effect.value;
            assert(post != -1);
            assert(pre != post);

            if (pre != -1) {
                propositions[var][post].always_produced_by.insert(op_id);
                propositions[var][pre].always_consumed_by.insert(op_id);
            } else {
                propositions[var][post].sometimes_produced_by.insert(op_id);
            }
        }
    }
}

void StateEquationConstraints::add_constraints(
    named_vector::NamedVector<lp::LPConstraint>& constraints,
    double infinity)
{
    for (vector<Proposition>& var_propositions : propositions) {
        for (Proposition& prop : var_propositions) {
            lp::LPConstraint constraint(-infinity, infinity);
            add_indices_to_constraint(constraint, prop.always_produced_by, 1.0);
            add_indices_to_constraint(
                constraint,
                prop.sometimes_produced_by,
                1.0);
            add_indices_to_constraint(
                constraint,
                prop.always_consumed_by,
                -1.0);
            if (!constraint.empty()) {
                prop.constraint_index = constraints.size();
                constraints.push_back(constraint);
            }
        }
    }
}

void StateEquationConstraints::initialize_constraints(
    const SharedAbstractTask& task,
    lp::LinearProgram& lp)
{
    if (log.is_at_least_normal()) {
        log << "Initializing constraints from state equation." << endl;
    }

    const auto& [variables, axioms, operators, goals] = to_refs(
        slice_shared<
            VariableSpace,
            AxiomSpace,
            ClassicalOperatorSpace,
            GoalFactList>(task));

    task_properties::verify_no_axioms(axioms);
    task_properties::verify_no_conditional_effects(operators);

    build_propositions(variables, operators);
    add_constraints(lp.get_constraints(), lp.get_infinity());

    // Initialize goal state.
    goal_state = vector<int>(variables.size(), numeric_limits<int>::max());
    for (const auto [var, value] : goals) { goal_state[var] = value; }
}

bool StateEquationConstraints::update_constraints(
    const State& state,
    lp::LPSolver& lp_solver)
{
    // Compute the bounds for the rows in the LP.
    for (size_t var = 0; var < propositions.size(); ++var) {
        int num_values = propositions[var].size();
        for (int value = 0; value < num_values; ++value) {
            const Proposition& prop = propositions[var][value];
            // Set row bounds.
            if (prop.constraint_index >= 0) {
                double lower_bound = 0;
                /* If we consider the current value of var, there must be an
                   additional consumer. */
                if (state[var] == value) { --lower_bound; }
                /* If we consider the goal value of var, there must be an
                   additional producer. */
                if (goal_state[var] == value) { ++lower_bound; }
                lp_solver.set_constraint_lower_bound(
                    prop.constraint_index,
                    lower_bound);
            }
        }
    }
    return false;
}

} // namespace downward::operator_counting
