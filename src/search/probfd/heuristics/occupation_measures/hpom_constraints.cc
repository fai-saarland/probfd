#include "probfd/heuristics/occupation_measures/hpom_constraints.h"

#include "probfd/heuristics/occupation_measures/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/cost_function.h"

#include "downward/lp/lp_solver.h"

#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include "downward/plugins/plugin.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <ranges>

namespace probfd {
namespace heuristics {
namespace occupation_measures {

namespace {

// Compute an explicit transition probability matrix
std::vector<std::vector<value_t>> get_transition_probs_explicit(
    const VariablesProxy& variables,
    const ProbabilisticOperatorProxy& op_proxy,
    std::set<int>& possibly_updated)
{
    const size_t num_variables = variables.size();

    std::vector<std::vector<value_t>> p(num_variables);

    for (std::size_t i = 0; i < num_variables; ++i) {
        p[i].resize(variables[i].get_domain_size() + 1, 0_vt);
        p[i].back() = 1_vt;
    }

    for (const ProbabilisticOutcomeProxy out : op_proxy.get_outcomes()) {
        const value_t prob = out.get_probability();

        for (const ProbabilisticEffectProxy effect : out.get_effects()) {
            const auto [var, val] = effect.get_fact().get_pair();

            possibly_updated.insert(var);
            auto& probs = p[var];
            probs[val] += prob;
            probs.back() -= prob;
        }
    }

    return p;
}
} // namespace

void HPOMGenerator::initialize_constraints(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function,
    lp::LinearProgram& lp)
{
    std::cout << "Initializing HPOM LP constraints..." << std::endl;

    utils::Timer timer;

    ProbabilisticTaskProxy task_proxy(*task);

    generate_hpom_lp(task_proxy, *task_cost_function, lp, offset_);

    std::cout << "Finished HPOM LP setup after " << timer << std::endl;
}

void HPOMGenerator::update_constraints(const State& state, lp::LPSolver& solver)
{
    // Set to initial state in LP
    for (size_t var = 0; var < state.size(); ++var) {
        const std::size_t index = offset_[var] + state[var].get_value();
        solver.set_constraint_upper_bound(index, 1.0);
    }
}

void HPOMGenerator::reset_constraints(const State& state, lp::LPSolver& solver)
{
    for (size_t var = 0; var < state.size(); ++var) {
        const std::size_t index = offset_[var] + state[var].get_value();
        solver.set_constraint_upper_bound(index, 0.0);
    }
}

void HPOMGenerator::generate_hpom_lp(
    ProbabilisticTaskProxy task_proxy,
    const FDRCostFunction& task_cost_function,
    lp::LinearProgram& lp,
    std::vector<int>& offset_)
{
    const value_t term_cost =
        task_cost_function.get_non_goal_termination_cost();

    if (term_cost != INFINITE_VALUE && term_cost != 1_vt) {
        std::cerr << "Termination costs beyond 1 (MaxProb) and +infinity (SSP) "
                     "currently unsupported in hpom implementation.";
        utils::exit_with(utils::ExitCode::SEARCH_UNSUPPORTED);
    }

    const bool maxprob = task_cost_function.get_non_goal_termination_cost();

    ::task_properties::verify_no_axioms(task_proxy);
    task_properties::verify_no_conditional_effects(task_proxy);

    auto& lp_variables = lp.get_variables();
    auto& constraints = lp.get_constraints();

    const VariablesProxy variables = task_proxy.get_variables();

    const std::size_t num_variables = variables.size();
    const double inf = lp.get_infinity();

    // Prepare fact variable offsets
    offset_.reserve(num_variables);

    std::size_t offset = 0;
    for (const VariableProxy variable : variables) {
        offset_.push_back(offset);
        offset += variable.get_domain_size();
    }

    const std::size_t num_facts = offset;

    // One flow constraint for every state of every atomic projections
    constraints.resize(num_facts, lp::LPConstraint(-inf, 0_vt));

    // Variable representing total inflow to artificial goal
    // Maximized in MaxProb, must be constant 1 for SSPs
    lp_variables.emplace_back(maxprob ? 0 : 1, 1, maxprob ? -1 : 0);

    std::vector<int> the_goal =
        pasmt_to_vector(task_proxy.get_goals(), num_variables);

    // Build flow contraint coefficients for dummy goal action
    for (const VariableProxy var : variables) {
        lp::LPConstraint& goal_constraint = constraints.emplace_back(0, 0);
        goal_constraint.insert(0, -1);
        lp::LPConstraint* flow = &constraints[offset_[var.get_id()]];

        if (the_goal[var.get_id()] == -1) {
            for (int val = 0; val != var.get_domain_size(); ++val) {
                const int lpvar = lp_variables.size();
                lp_variables.emplace_back(0, inf, 0);
                flow[val].insert(lpvar, 1);       // outflow for goal states
                goal_constraint.insert(lpvar, 1); // inflow for dummy goal state
            }
        } else {
            const int val = the_goal[var.get_id()];
            const int lpvar = lp_variables.size();
            lp_variables.emplace_back(0, inf, 0);
            flow[val].insert(lpvar, 1);
            goal_constraint.insert(lpvar, 1);
        }
    }

    // Now ordinary actions
    for (const ProbabilisticOperatorProxy& op : task_proxy.get_operators()) {
        const auto cost = maxprob ? 0 : op.get_cost();

        // Get dense precondition
        const std::vector<int> pre =
            pasmt_to_vector(op.get_preconditions(), num_variables);

        // Get transition matrix and possibly updated variables
        std::set<int> possibly_updated;
        const std::vector<std::vector<value_t>> post =
            get_transition_probs_explicit(variables, op, possibly_updated);

        // For tying constraints, contains lp variable ranges of projections
        std::vector<std::pair<int, int>> tieing_equality;

        // Build flow constraint coefficients...
        for (const int var : possibly_updated) {
            std::pair<int, int> var_range(lp_variables.size(), 0);
            lp::LPConstraint* flow = &constraints[offset_[var]];

            const std::size_t domain = variables[var].get_domain_size();
            const auto& tr_probs = post[var];

            // Populate flow constraints
            if (pre[var] == -1) {
                for (std::size_t i = 0; i < domain; ++i) {
                    const auto p_self_loop = tr_probs[i] + tr_probs.back();

                    // Occupation measure / flow variable x_{d, a}
                    const int lpvar = lp_variables.size();
                    lp_variables.emplace_back(0, inf, 0);

                    // Outflow
                    flow[i].insert(lpvar, 1 - p_self_loop);

                    // Inflows
                    for (std::size_t j = 0; j < domain; ++j) {
                        if (j == i) continue;

                        const value_t prob = tr_probs[j];
                        if (prob > 0_vt) {
                            flow[j].insert(lpvar, -prob);
                        }
                    }
                }
            } else {
                const std::size_t i = pre[var];
                const auto p_self_loop = tr_probs[i] + tr_probs.back();

                // Occupation measure / flow variable x_{d, a}
                const int lpvar = lp_variables.size();
                lp_variables.emplace_back(0, inf, 0);

                // Outflow
                flow[i].insert(lpvar, 1 - p_self_loop);

                // Inflows
                for (std::size_t j = 0; j < domain; ++j) {
                    if (j == i) continue;

                    const value_t prob = tr_probs[j];
                    if (prob > 0) {
                        flow[j].insert(lpvar, -prob);
                    }
                }
            }

            var_range.second = lp_variables.size();

            tieing_equality.push_back(var_range);
        }

        // Build tying constraints, tie everything to first projection
        if (!tieing_equality.empty()) {
            const auto& base_range = tieing_equality[0];

            // Set objective coefficients for occ. measures of first projection
            for (int i = base_range.first; i < base_range.second; ++i) {
                lp_variables[i].objective_coefficient = cost;
            }

            for (std::size_t j = 1; j < tieing_equality.size(); ++j) {
                const auto& tieing_eq = tieing_equality[j];

                auto& tieing_constraint = constraints.emplace_back(0, 0);

                for (int i = base_range.first; i < base_range.second; ++i) {
                    tieing_constraint.insert(i, 1);
                }

                for (int i = tieing_eq.first; i < tieing_eq.second; ++i) {
                    tieing_constraint.insert(i, -1);
                }
            }
        }
    }
}

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd
