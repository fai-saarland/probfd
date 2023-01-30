#include "probfd/heuristics/occupation_measure/occupation_measure_heuristic.h"

#include "option_parser.h"
#include "plugin.h"

#include "utils/system.h"
#include "utils/timer.h"

#include "probfd/analysis_objectives/expected_cost_objective.h"
#include "probfd/analysis_objectives/goal_probability_objective.h"

#include "probfd/task_utils/task_properties.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>

namespace probfd {
namespace heuristics {
namespace occupation_measure_heuristic {

namespace {
// Explicit goal values (-1 if variable not required)
std::vector<int> get_goal_explicit(const ProbabilisticTaskProxy& task_proxy)
{
    std::vector<int> the_goal(task_proxy.get_variables().size(), -1);

    for (const FactProxy fact : task_proxy.get_goals()) {
        the_goal[fact.get_variable().get_id()] = fact.get_value();
    }

    return the_goal;
}

// Explicit precondition values (-1 if no precondition for variable)
std::vector<int> get_precondition_explicit(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilisticOperatorProxy& op_proxy)
{
    std::vector<int> pre(task_proxy.get_variables().size(), -1);

    for (const FactProxy fact : op_proxy.get_preconditions()) {
        pre[fact.get_variable().get_id()] = fact.get_value();
    }

    return pre;
}

// Compute an explicit transition probability matrix
std::vector<std::vector<value_type::value_t>> get_transition_probs_explicit(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilisticOperatorProxy& op_proxy,
    std::set<int>& possibly_updated)
{
    const VariablesProxy variables = task_proxy.get_variables();
    const size_t num_variables = variables.size();

    std::vector<std::vector<value_type::value_t>> p(num_variables);

    for (std::size_t i = 0; i < num_variables; ++i) {
        p[i].resize(variables[i].get_domain_size() + 1, value_type::zero);
        p[i].back() = value_type::one;
    }

    for (const ProbabilisticOutcomeProxy& out : op_proxy.get_outcomes()) {
        const value_type::value_t prob = out.get_probability();

        for (const ProbabilisticEffectProxy effect : out.get_effects()) {
            const FactProxy fact = effect.get_fact();
            int var = fact.get_variable().get_id();
            int val = fact.get_value();

            possibly_updated.insert(var);
            auto& probs = p[var];
            probs[val] += prob;
            probs.back() -= prob;
        }
    }

    return p;
}
} // namespace

void ProjectionOccupationMeasureHeuristic::generate_hpom_lp(
    const ProbabilisticTaskProxy& task_proxy,
    lp::LPSolver& lp_solver_,
    named_vector::NamedVector<lp::LPVariable>& lp_vars,
    named_vector::NamedVector<lp::LPConstraint>& constraints,
    std::vector<int>& offset_,
    bool maxprob)
{
    assert(lp_vars.empty() && constraints.empty());

    ::task_properties::verify_no_axioms(task_proxy);
    task_properties::verify_no_conditional_effects(task_proxy);

    const VariablesProxy variables = task_proxy.get_variables();
    const GoalsProxy goals = task_proxy.get_goals();

    const std::size_t num_variables = variables.size();
    const double inf = lp_solver_.get_infinity();

    // Prepare fact variable offsets
    offset_.reserve(num_variables);

    std::size_t offset = 0;
    for (const VariableProxy variable : variables) {
        offset_.push_back(offset);
        offset += variable.get_domain_size();
    }

    const std::size_t num_facts = offset;

    // One flow constraint for every state of every atomic projections
    constraints.resize(num_facts, lp::LPConstraint(-inf, value_type::zero));

    // Variable representing total inflow to artificial goal
    // Maximized in MaxProb, must be constant 1 for SSPs
    lp_vars.emplace_back(maxprob ? 0 : 1, 1, maxprob ? 1 : 0);

    std::vector<int> the_goal = get_goal_explicit(task_proxy);

    // Build flow contraint coefficients for dummy goal action
    for (const VariableProxy var : variables) {
        lp::LPConstraint& goal_constraint = constraints.emplace_back(0, 0);
        goal_constraint.insert(0, -1);
        lp::LPConstraint* flow = &constraints[offset_[var.get_id()]];

        if (the_goal[var.get_id()] == -1) {
            for (int val = 0; val != var.get_domain_size(); ++val) {
                const int lpvar = lp_vars.size();
                lp_vars.emplace_back(0, inf, 0);
                flow[val].insert(lpvar, 1);       // outflow for goal states
                goal_constraint.insert(lpvar, 1); // inflow for dummy goal state
            }
        } else {
            const int val = the_goal[var.get_id()];
            const int lpvar = lp_vars.size();
            lp_vars.emplace_back(0, inf, 0);
            flow[val].insert(lpvar, 1);
            goal_constraint.insert(lpvar, 1);
        }
    }

    // Now ordinary actions
    for (const ProbabilisticOperatorProxy& op : task_proxy.get_operators()) {
        const auto reward = maxprob ? 0 : op.get_reward();

        // Get dense precondition
        const std::vector<int> pre = get_precondition_explicit(task_proxy, op);

        // Get transition matrix and possibly updated variables
        std::set<int> possibly_updated;
        const std::vector<std::vector<value_type::value_t>> post =
            get_transition_probs_explicit(task_proxy, op, possibly_updated);

        // For tying constraints, contains lp variable ranges of projections
        std::vector<std::pair<int, int>> tieing_equality;

        // Build flow constraint coefficients...
        for (const int var : possibly_updated) {
            std::pair<int, int> var_range(lp_vars.size(), 0);
            lp::LPConstraint* flow = &constraints[offset_[var]];

            const std::size_t domain = variables[var].get_domain_size();
            const auto& tr_probs = post[var];

            // Populate flow constraints
            if (pre[var] == -1) {
                for (std::size_t i = 0; i < domain; ++i) {
                    const auto p_self_loop = tr_probs[i] + tr_probs.back();

                    // Occupation measure / flow variable x_{d, a}
                    const int lpvar = lp_vars.size();
                    lp_vars.emplace_back(0, inf, 0);

                    // Outflow
                    flow[i].insert(lpvar, 1 - p_self_loop);

                    // Inflows
                    for (std::size_t j = 0; j < domain; ++j) {
                        if (j == i) continue;

                        const value_type::value_t prob = tr_probs[j];
                        if (prob > value_type::zero) {
                            flow[j].insert(lpvar, -prob);
                        }
                    }
                }
            } else {
                const std::size_t i = pre[var];
                const auto p_self_loop = tr_probs[i] + tr_probs.back();

                // Occupation measure / flow variable x_{d, a}
                const int lpvar = lp_vars.size();
                lp_vars.emplace_back(0, inf, 0);

                // Outflow
                flow[i].insert(lpvar, 1 - p_self_loop);

                // Inflows
                for (std::size_t j = 0; j < domain; ++j) {
                    if (j == i) continue;

                    const value_type::value_t prob = tr_probs[j];
                    if (prob > 0) {
                        flow[j].insert(lpvar, -prob);
                    }
                }
            }

            var_range.second = lp_vars.size();

            tieing_equality.push_back(var_range);
        }

        // Build tying constraints, tie everything to first projection
        if (!tieing_equality.empty()) {
            const auto& base_range = tieing_equality[0];

            // Set objective coefficients for occ. measures of first projection
            for (int i = base_range.first; i < base_range.second; ++i) {
                lp_vars[i].objective_coefficient = reward;
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

ProjectionOccupationMeasureHeuristic::ProjectionOccupationMeasureHeuristic(
    const options::Options& opts)
    : TaskDependentHeuristic(opts)
    , lp_solver_(opts.get<lp::LPSolverType>("lpsolver"))
    , is_maxprob_(
          std::dynamic_pointer_cast<
              analysis_objectives::GoalProbabilityObjective>(
              g_analysis_objective) != nullptr)
{
    using namespace analysis_objectives;

    std::cout << "Initializing projection occupation measure heuristic ..."
              << std::endl;
    utils::Timer timer;

    named_vector::NamedVector<lp::LPVariable> lp_vars;
    named_vector::NamedVector<lp::LPConstraint> constraints;

    generate_hpom_lp(
        task_proxy,
        lp_solver_,
        lp_vars,
        constraints,
        offset_,
        is_maxprob_);

    lp_solver_.load_problem(lp::LinearProgram(
        lp::LPObjectiveSense::MAXIMIZE,
        std::move(lp_vars),
        std::move(constraints),
        lp_solver_.get_infinity()));

    std::cout << "Finished POM LP setup after " << timer << std::endl;
}

EvaluationResult
ProjectionOccupationMeasureHeuristic::evaluate(const State& state) const
{
    using namespace analysis_objectives;

    const size_t num_variables = task_proxy.get_variables().size();

    // Set to initial state in LP
    for (size_t var = 0; var < num_variables; ++var) {
        const std::size_t index = offset_[var] + state[var].get_value();
        lp_solver_.set_constraint_upper_bound(index, 1.0);
    }

    lp_solver_.solve();

    EvaluationResult result;

    if (is_maxprob_) {
        assert(lp_solver_.has_optimal_solution());

        const double estimate = lp_solver_.get_objective_value();
        result = EvaluationResult(estimate == value_type::zero, estimate);

    } else {
        bool was_feasible = lp_solver_.has_optimal_solution();

        // Costs are negative rewards, return negative solution.
        const double estimate =
            was_feasible ? lp_solver_.get_objective_value() : -value_type::inf;
        result = EvaluationResult(!was_feasible, estimate);
    }

    for (size_t var = 0; var < num_variables; ++var) {
        const std::size_t index = offset_[var] + state[var].get_value();
        lp_solver_.set_constraint_upper_bound(index, 0.0);
    }

    return result;
}

void ProjectionOccupationMeasureHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    TaskDependentHeuristic::add_options_to_parser(parser);
    lp::add_lp_solver_option_to_parser(parser);
}

static Plugin<TaskStateEvaluator> _plugin(
    "hpom",
    options::parse<TaskStateEvaluator, ProjectionOccupationMeasureHeuristic>);

} // namespace occupation_measure_heuristic
} // namespace heuristics
} // namespace probfd
