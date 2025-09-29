#include "probfd/occupation_measures/hroc_constraints.h"

#include "probfd/occupation_measures/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/cost_function.h"
#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/algorithms/named_vector.h"

#include "downward/lp/lp_solver.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include "downward/state.h"
#include "probfd/probabilistic_operator_space.h"
#include "probfd/termination_costs.h"

#include <cassert>
#include <iostream>
#include <memory>

using namespace downward;

namespace probfd::occupation_measures {

void HROCGenerator::initialize_constraints(
    const SharedProbabilisticTask& task,
    lp::LinearProgram& lp)
{
    const auto& variables = get_variables(task);
    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);
    const auto& goals = get_goal(task);
    const auto& cost_function = get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    const value_t term_cost = term_costs.get_non_goal_termination_cost();

    if (term_cost != INFINITE_VALUE && term_cost != 1_vt) {
        throw utils::UnsupportedError(
            "Termination costs beyond 1 (MaxProb) and +infinity (SSP) "
            "currently unsupported in hroc implementation.");
    }

    const bool maxprob = term_costs.get_non_goal_termination_cost() == 1_vt;

    ::task_properties::verify_no_axioms(axioms);
    task_properties::verify_no_conditional_effects(operators);

    std::println(
        std::cout,
        "Initializing regrouped operator counting heuristic...");

    utils::Timer timer;

    // Construct LP...
    const std::size_t num_variables = variables.size();

    const double inf = lp.get_infinity();

    // Set up net change constraint offsets, one constraint per fact
    ncc_offsets_.reserve(num_variables);

    std::size_t offset = 0;
    for (const VariableProxy variable : variables) {
        ncc_offsets_.push_back(offset);
        offset += variable.get_domain_size();
    }

    auto& lp_variables = lp.get_variables();
    auto& constraints = lp.get_constraints();

    const std::size_t num_facts = offset;

    constraints.resize(num_facts, lp::LPConstraint(0.0, inf));

    // Insert flow absorption variable into goal fact constraints
    if (maxprob) {
        lp_variables.emplace_back(0, 1, -1);
    } else {
        lp_variables.emplace_back(1, 1, 0);
    }

    for (const auto [var, value] : goals) {
        constraints[ncc_offsets_[var] + value].insert(0, -1);
    }

    for (const ProbabilisticOperatorProxy op : operators) {
        const value_t cost =
            maxprob ? 0_vt : cost_function.get_operator_cost(op.get_id());

        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();

        assert(outcomes.size() >= 1);

        const auto first_eff_probability = outcomes[0].get_probability();
        const int first_eff_lpvar = lp_variables.size();

        // Get explicit precondition
        const std::vector<int> pre =
            pasmt_to_vector(op.get_preconditions(), num_variables);

        for (std::size_t i = 0; i < outcomes.size(); ++i) {
            const ProbabilisticOutcomeProxy outcome = outcomes[i];
            const value_t probability = outcome.get_probability();

            const int lp_var = lp_variables.size();

            // introduce variable Y_{a,e}
            lp_variables.emplace_back(0, inf, cost);

            for (const auto& effect : outcome.get_effects()) {
                const FactPair fact = effect.get_fact();
                const int var = fact.var;
                const int val = fact.value;

                auto* var_constraints = &constraints[0] + ncc_offsets_[var];

                // Always produces / Sometimes produces
                var_constraints[val].insert(lp_var, 1);

                const int pre_val = pre[var];

                if (pre_val != -1) {
                    // Always consumes
                    var_constraints[pre_val].insert(lp_var, -1);
                }
            }

            // Skip first variable for regrouping constraints
            if (i == 0) { continue; }

            // Set up regrouping constraint coefficients
            auto& rg_constraint = constraints.emplace_back(0, 0);
            rg_constraint.insert(first_eff_lpvar, probability);
            rg_constraint.insert(lp_var, -first_eff_probability);
        }
    }

    std::println(std::cout, "Finished ROC LP setup after {}", timer());
}

void HROCGenerator::update_constraints(const State& state, lp::LPSolver& solver)
{
    // Set outflow of 1 for all state facts
    for (std::size_t var = 0; var < state.size(); ++var) {
        const int c_index = ncc_offsets_[var] + state[var];
        solver.set_constraint_lower_bound(c_index, -1.0);
    }
}

void HROCGenerator::reset_constraints(const State& state, lp::LPSolver& solver)
{
    // Reset the coefficients to zero
    for (std::size_t var = 0; var < state.size(); ++var) {
        const int c_index = ncc_offsets_[var] + state[var];
        solver.set_constraint_lower_bound(c_index, 0.0);
    }
}

std::unique_ptr<ConstraintGenerator>
HROCGeneratorFactory::construct_constraint_generator(
    const SharedProbabilisticTask&)
{
    return std::make_unique<HROCGenerator>();
}

} // namespace probfd::occupation_measures
