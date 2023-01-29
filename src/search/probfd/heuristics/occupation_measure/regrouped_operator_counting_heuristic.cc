#include "probfd/heuristics/occupation_measure/regrouped_operator_counting_heuristic.h"

#include "option_parser.h"
#include "plugin.h"

#include "utils/system.h"
#include "utils/timer.h"

#include "probfd/cost_models/maxprob_cost_model.h"

#include "probfd/task_utils/task_properties.h"

#include <algorithm>
#include <cassert>
#include <memory>

namespace probfd {
namespace heuristics {
namespace occupation_measure_heuristic {

namespace {

std::vector<int> make_explicit(
    const ProbabilisticTaskProxy& task_proxy,
    const OperatorPreconditionsProxy& preconditions_proxy)
{
    std::vector<int> pre(task_proxy.get_variables().size(), -1);

    for (const FactProxy fact : preconditions_proxy) {
        pre[fact.get_variable().get_id()] = fact.get_value();
    }

    return pre;
}

// Explicit precondition values (-1 if no precondition for variable)
std::vector<int> get_precondition_explicit(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilisticOperatorProxy& op)
{
    return make_explicit(task_proxy, op.get_preconditions());
}

} // namespace

RegroupedOperatorCountingHeuristic::RegroupedOperatorCountingHeuristic(
    const options::Options& opts)
    : TaskDependentHeuristic(opts)
    , lp_solver_(opts.get<lp::LPSolverType>("lpsolver"))
    , is_maxprob(
          std::dynamic_pointer_cast<cost_models::MaxProbCostModel>(
              g_cost_model) != nullptr)
{
    ::task_properties::verify_no_axioms(task_proxy);
    task_properties::verify_no_conditional_effects(task_proxy);

    std::cout << "Initializing regrouped operator counting heuristic..."
              << std::endl;

    utils::Timer timer;

    // Construct LP...

    const VariablesProxy variables = task_proxy.get_variables();

    const std::size_t num_variables = variables.size();

    this->reset_indices_.reserve(2 * num_variables);

    const double inf = lp_solver_.get_infinity();

    named_vector::NamedVector<lp::LPVariable> lp_vars;
    named_vector::NamedVector<lp::LPConstraint> constraints;

    // Set up net change constraint offsets, one constraint per fact
    ncc_offsets_.reserve(num_variables);

    std::size_t offset = 0;
    for (VariableProxy variable : variables) {
        ncc_offsets_.push_back(offset);
        offset += variable.get_domain_size();
    }

    const std::size_t num_facts = offset;

    constraints.resize(num_facts, lp::LPConstraint(0.0, inf));

    // Insert flow absorption variable into goal fact constraints
    if (is_maxprob) {
        lp_vars.emplace_back(0, 1, -1);
    } else {
        lp_vars.emplace_back(1, 1, 0);
    }

    for (const FactProxy goal_fact : task_proxy.get_goals()) {
        const std::size_t off = ncc_offsets_[goal_fact.get_variable().get_id()];
        constraints[off + goal_fact.get_value()].insert(0, -1);
    }

    for (const ProbabilisticOperatorProxy op : task_proxy.get_operators()) {
        const int cost = is_maxprob ? 0_vt : op.get_cost();

        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();

        assert(outcomes.size() >= 1);

        const auto first_eff_probability = outcomes[0].get_probability();
        const int first_eff_lpvar = lp_vars.size();

        // Get explicit precondition
        const std::vector<int> pre = get_precondition_explicit(task_proxy, op);

        for (std::size_t i = 0; i < outcomes.size(); ++i) {
            const ProbabilisticOutcomeProxy outcome = outcomes[i];
            const value_t probability = outcome.get_probability();

            const int lp_var = lp_vars.size();

            // introduce variable Y_{a,e}
            lp_vars.emplace_back(0, inf, cost);

            for (const auto& effect : outcome.get_effects()) {
                const FactProxy fact = effect.get_fact();
                const int var = fact.get_variable().get_id();
                const int val = fact.get_value();

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
            if (i == 0) {
                continue;
            }

            // Set up regrouping constraint coefficients
            auto& rg_constraint = constraints.emplace_back(0, 0);
            rg_constraint.insert(first_eff_lpvar, probability);
            rg_constraint.insert(lp_var, -first_eff_probability);
        }
    }

    lp_solver_.load_problem(lp::LinearProgram(
        lp::LPObjectiveSense::MINIMIZE,
        std::move(lp_vars),
        std::move(constraints),
        inf));

    std::cout << "Finished ROC LP setup after " << timer << std::endl;
}

EvaluationResult
RegroupedOperatorCountingHeuristic::evaluate(const State& state) const
{
    // Set outflow of 1 for all state facts
    for (std::size_t var = 0; var < task_proxy.get_variables().size(); ++var) {
        const int c_index = ncc_offsets_[var] + state[var].get_value();
        lp_solver_.set_constraint_lower_bound(c_index, -1);
        reset_indices_.push_back(c_index);
    }

    lp_solver_.solve();

    EvaluationResult result;

    if (is_maxprob) {
        assert(lp_solver_.has_optimal_solution());
        const double estimate = lp_solver_.get_objective_value();
        result = EvaluationResult(estimate == 0_vt, estimate);
    } else {
        bool was_feasible = lp_solver_.has_optimal_solution();
        assert(lp_solver_.has_optimal_solution());
        const double estimate =
            was_feasible ? lp_solver_.get_objective_value() : INFINITE_VALUE;
        result = EvaluationResult(!was_feasible, estimate);
    }

    // Reset the objective coefficients
    for (int idx : reset_indices_) {
        lp_solver_.set_constraint_lower_bound(idx, 0.0);
    }

    reset_indices_.clear();

    return result;
}

void RegroupedOperatorCountingHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    TaskDependentHeuristic::add_options_to_parser(parser);
    lp::add_lp_solver_option_to_parser(parser);
}

static Plugin<TaskStateEvaluator> _plugin(
    "hroc",
    options::parse<TaskStateEvaluator, RegroupedOperatorCountingHeuristic>);

} // namespace occupation_measure_heuristic
} // namespace heuristics
} // namespace probfd
