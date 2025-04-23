#include "downward/potentials/potential_optimizer.h"

#include "downward/potentials/potential_function.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/collections.h"
#include "downward/utils/memory.h"
#include "downward/utils/system.h"

#include <limits>
#include <unordered_map>

using namespace std;
using downward::utils::ExitCode;

namespace downward::potentials {
static int get_undefined_value(VariableProxy var)
{
    return var.get_domain_size();
}

PotentialOptimizer::PotentialOptimizer(
    const shared_ptr<AbstractTask>& transform,
    lp::LPSolverType lpsolver,
    double max_potential)
    : task(transform)
    , lp_solver(lpsolver)
    , max_potential(max_potential)
    , num_lp_vars(0)
{
    task_properties::verify_no_axioms(*task);
    task_properties::verify_no_conditional_effects(*task);
    initialize();
}

void PotentialOptimizer::initialize()
{
    VariablesProxy vars = task->get_variables();
    lp_var_ids.resize(vars.size());
    fact_potentials.resize(vars.size());
    for (VariableProxy var : vars) {
        // Add LP variable for "undefined" value.
        lp_var_ids[var.get_id()].resize(var.get_domain_size() + 1);
        for (int val = 0; val < var.get_domain_size() + 1; ++val) {
            lp_var_ids[var.get_id()][val] = num_lp_vars++;
        }
        fact_potentials[var.get_id()].resize(var.get_domain_size());
    }
    construct_lp();
}

bool PotentialOptimizer::has_optimal_solution() const
{
    return lp_solver.has_optimal_solution();
}

void PotentialOptimizer::optimize_for_state(const State& state)
{
    optimize_for_samples({state});
}

int PotentialOptimizer::get_lp_var_id(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.var, lp_var_ids));
    assert(utils::in_bounds(fact.value, lp_var_ids[fact.var]));
    return lp_var_ids[fact.var][fact.value];
}

void PotentialOptimizer::optimize_for_all_states()
{
    if (!potentials_are_bounded()) {
        cerr << "Potentials must be bounded for all-states LP." << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
    vector<double> coefficients(num_lp_vars, 0.0);
    for (FactProxy fact : task->get_variables().get_facts()) {
        coefficients[get_lp_var_id(fact.get_pair())] =
            1.0 / fact.get_variable().get_domain_size();
    }
    lp_solver.set_objective_coefficients(coefficients);
    solve_and_extract();
    if (!has_optimal_solution()) {
        ABORT("all-states LP unbounded even though potentials are bounded.");
    }
}

void PotentialOptimizer::optimize_for_samples(const vector<State>& samples)
{
    vector<double> coefficients(num_lp_vars, 0.0);
    for (const State& state : samples) {
        for (FactPair fact : state | as_fact_pair_set) {
            coefficients[get_lp_var_id(fact)] += 1.0;
        }
    }
    lp_solver.set_objective_coefficients(coefficients);
    solve_and_extract();
}

shared_ptr<AbstractTask> PotentialOptimizer::get_task() const
{
    return task;
}

bool PotentialOptimizer::potentials_are_bounded() const
{
    return max_potential != numeric_limits<double>::infinity();
}

void PotentialOptimizer::construct_lp()
{
    double infinity = lp_solver.get_infinity();
    double upper_bound = (potentials_are_bounded() ? max_potential : infinity);

    const auto variables = task->get_variables();

    named_vector::NamedVector<lp::LPVariable> lp_variables;
    lp_variables.reserve(num_lp_vars);
    for (int lp_var_id = 0; lp_var_id < num_lp_vars; ++lp_var_id) {
        // Use dummy coefficient for now. Adapt coefficient later.
        lp_variables.emplace_back(-infinity, upper_bound, 1.0);
    }

    named_vector::NamedVector<lp::LPConstraint> lp_constraints;
    for (OperatorProxy op : task->get_operators()) {
        // Create constraint:
        // Sum_{V in vars(eff(o))} (P_{V=pre(o)[V]} - P_{V=eff(o)[V]}) <=
        // cost(o)
        unordered_map<int, int> var_to_precondition;
        for (const auto [var, value] : op.get_preconditions()) {
            var_to_precondition[var] = value;
        }
        lp::LPConstraint constraint(
            -infinity,
            task->get_operator_cost(op.get_id()));
        vector<pair<int, int>> coefficients;
        for (auto effect : op.get_effects()) {
            int var_id = effect.get_fact().var;

            // Set pre to pre(op) if defined, otherwise to u = |dom(var)|.
            int pre = -1;
            auto it = var_to_precondition.find(var_id);
            if (it == var_to_precondition.end()) {
                pre = get_undefined_value(variables[var_id]);
            } else {
                pre = it->second;
            }

            int post = effect.get_fact().value;
            int pre_lp = lp_var_ids[var_id][pre];
            int post_lp = lp_var_ids[var_id][post];
            assert(pre_lp != post_lp);
            coefficients.emplace_back(pre_lp, 1);
            coefficients.emplace_back(post_lp, -1);
        }
        sort(coefficients.begin(), coefficients.end());
        for (const auto& coeff : coefficients)
            constraint.insert(coeff.first, coeff.second);
        lp_constraints.push_back(constraint);
    }

    /* Create full goal state. Use value |dom(V)| as "undefined" value
       for variables V undefined in the goal. */
    vector<int> goal(variables.size(), -1);
    for (const auto [var, value] : task->get_goals()) { goal[var] = value; }
    for (VariableProxy var : variables) {
        if (goal[var.get_id()] == -1)
            goal[var.get_id()] = get_undefined_value(var);
    }

    for (VariableProxy var : variables) {
        /*
          Create constraint (using variable bounds): P_{V=goal[V]} = 0
          When each variable has a goal value (including the
          "undefined" value), this is equivalent to the goal-awareness
          constraint \sum_{fact in goal} P_fact <= 0. We can't set the
          potential of one goal fact to +2 and another to -2, but if
          all variables have goal values, this is not beneficial
          anyway.
        */
        int var_id = var.get_id();
        lp::LPVariable& lp_var = lp_variables[lp_var_ids[var_id][goal[var_id]]];
        lp_var.lower_bound = 0;
        lp_var.upper_bound = 0;

        int undef_val_lp = lp_var_ids[var_id][get_undefined_value(var)];
        for (int val = 0; val < var.get_domain_size(); ++val) {
            int val_lp = lp_var_ids[var_id][val];
            // Create constraint: P_{V=v} <= P_{V=u}
            // Note that we could eliminate variables P_{V=u} if V is
            // undefined in the goal.
            lp::LPConstraint constraint(-infinity, 0);
            constraint.insert(val_lp, 1);
            constraint.insert(undef_val_lp, -1);
            lp_constraints.push_back(constraint);
        }
    }
    lp::LinearProgram lp(
        lp::LPObjectiveSense::MAXIMIZE,
        std::move(lp_variables),
        std::move(lp_constraints),
        infinity);
    lp_solver.load_problem(lp);
}

void PotentialOptimizer::solve_and_extract()
{
    lp_solver.solve();
    if (has_optimal_solution()) { extract_lp_solution(); }
}

void PotentialOptimizer::extract_lp_solution()
{
    assert(has_optimal_solution());
    const vector<double> solution = lp_solver.extract_solution();
    for (FactProxy fact : task->get_variables().get_facts()) {
        fact_potentials[fact.get_variable().get_id()][fact.get_value()] =
            solution[get_lp_var_id(fact.get_pair())];
    }
}

unique_ptr<PotentialFunction> PotentialOptimizer::get_potential_function() const
{
    assert(has_optimal_solution());
    return std::make_unique<PotentialFunction>(fact_potentials);
}
} // namespace downward::potentials
