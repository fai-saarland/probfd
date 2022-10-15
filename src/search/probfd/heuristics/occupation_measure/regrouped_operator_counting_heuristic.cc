#include "probfd/heuristics/occupation_measure/regrouped_operator_counting_heuristic.h"

#include "global_operator.h"
#include "globals.h"
#include "option_parser.h"
#include "plugin.h"

#include "utils/system.h"
#include "utils/timer.h"

#include "probfd/analysis_objectives/expected_cost_objective.h"
#include "probfd/analysis_objectives/goal_probability_objective.h"

#include "probfd/globals.h"
#include "probfd/probabilistic_operator.h"

#include <algorithm>
#include <cassert>
#include <memory>

namespace probfd {
namespace occupation_measure_heuristic {

namespace {

template <typename T>
std::vector<int> make_explicit(const std::vector<T>& partial_state)
{
    std::vector<int> pre(g_variable_domain.size(), -1);

    for (const auto& [var, val] : partial_state) {
        pre[var] = val;
    }

    return pre;
}

// Explicit precondition values (-1 if no precondition for variable)
std::vector<int> get_precondition_explicit(const ProbabilisticOperator& op)
{
    return make_explicit(op.get_preconditions());
}

} // namespace

RegroupedOperatorCountingHeuristic::RegroupedOperatorCountingHeuristic(
    const options::Options& opts)
    : lp_solver_(lp::LPSolverType(opts.get_enum("lpsolver")))
    , is_maxprob(
          std::dynamic_pointer_cast<
              analysis_objectives::GoalProbabilityObjective>(
              g_analysis_objective) != nullptr)
{
    ::verify_no_axioms_no_conditional_effects();

    std::cout << "Initializing regrouped operator counting heuristic..."
              << std::endl;

    utils::Timer timer;

    // Construct LP...

    const std::size_t num_variables = g_variable_domain.size();

    this->reset_indices_.reserve(2 * num_variables);

    const double inf = lp_solver_.get_infinity();

    std::vector<lp::LPVariable> lp_vars;
    std::vector<lp::LPConstraint> constraints;

    // Set up net change constraint offsets, one constraint per fact
    ncc_offsets_.reserve(num_variables);

    std::size_t offset = 0;
    ncc_offsets_.push_back(offset);
    for (std::size_t var = 0; var < num_variables - 1; ++var) {
        offset += g_variable_domain[var];
        ncc_offsets_.push_back(offset);
    }

    const std::size_t num_facts = offset + g_variable_domain.back();

    constraints.resize(num_facts, lp::LPConstraint(0.0, inf));

    // Insert flow absorption variable into goal fact constraints
    if (is_maxprob) {
        lp_vars.emplace_back(0, 1, 1);
    } else {
        lp_vars.emplace_back(1, 1, 0);
    }

    for (const auto& goal_fact : g_goal) {
        const std::size_t offset = ncc_offsets_[goal_fact.first];
        constraints[offset + goal_fact.second].insert(0, -1);
    }

    for (const ProbabilisticOperator& op : g_operators) {
        const int reward = is_maxprob ? value_type::zero : op.get_reward();

        assert(op.num_outcomes() >= 1);

        const auto first_eff_probability = op[0].prob;
        const int first_eff_lpvar = lp_vars.size();

        // Get explicit precondition
        const std::vector<int> pre = get_precondition_explicit(op);

        for (std::size_t i = 0; i < op.num_outcomes(); ++i) {
            const ProbabilisticOutcome& o = op[i];
            const auto probability = o.prob;
            const auto& effects = o.effects();

            const int lp_var = lp_vars.size();

            // introduce variable Y_{a,e}
            lp_vars.emplace_back(0, inf, reward);

            for (const auto& eff : effects) {
                const int var = eff.var;
                const int val = eff.val;

                auto* var_constraints = constraints.data() + ncc_offsets_[var];

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

    lp_solver_.load_problem(
        lp::LPObjectiveSense::MAXIMIZE,
        lp_vars,
        constraints);

    std::cout << "Finished ROC LP setup after " << timer << std::endl;
}

EvaluationResult
RegroupedOperatorCountingHeuristic::evaluate(const GlobalState& state) const
{
    // Set outflow of 1 for all state facts
    for (std::size_t var = 0; var < g_variable_domain.size(); ++var) {
        const int c_index = ncc_offsets_[var] + state[var];
        lp_solver_.set_constraint_lower_bound(c_index, -1);
        reset_indices_.push_back(c_index);
    }

    lp_solver_.solve();

    EvaluationResult result;

    if (is_maxprob) {
        assert(lp_solver_.has_optimal_solution());
        const double estimate = lp_solver_.get_objective_value();
        result = EvaluationResult(estimate == value_type::zero, estimate);
    } else {
        bool was_feasible = lp_solver_.has_optimal_solution();
        assert(lp_solver_.has_optimal_solution());
        const double estimate =
            was_feasible ? lp_solver_.get_objective_value() : -value_type::inf;
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
    lp::add_lp_solver_option_to_parser(parser);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "hroc",
    options::parse<GlobalStateEvaluator, RegroupedOperatorCountingHeuristic>);

} // namespace occupation_measure_heuristic
} // namespace probfd
