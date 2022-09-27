#include "regrouped_operator_counting_heuristic.h"

#include "../../../global_operator.h"
#include "../../../globals.h"
#include "../../../option_parser.h"
#include "../../../plugin.h"
#include "../../../utils/system.h"
#include "../../../utils/timer.h"
#include "../../analysis_objectives/expected_cost_objective.h"
#include "../../analysis_objectives/goal_probability_objective.h"
#include "../../globals.h"
#include "../../probabilistic_operator.h"

#include <algorithm>
#include <cassert>
#include <memory>

namespace probabilistic {
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

// Explicit goal values (-1 if not a goal variable)
std::vector<int> get_goal_explicit()
{
    return make_explicit(g_goal);
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

    if (is_maxprob) {
        load_maxprob_lp();
    } else {
        load_expcost_lp();
    }

    std::cout << "Finished ROC LP setup after " << timer << std::endl;
}

EvaluationResult
RegroupedOperatorCountingHeuristic::evaluate(const GlobalState& state) const
{
    const std::size_t num_variables = g_variable_domain.size();

    if (is_maxprob) {
        for (std::size_t i = 0; i < g_goal.size(); ++i) {
            const auto& goal_fact = g_goal[i];
            if (state[goal_fact.first] == goal_fact.second) {
                const int c_index = num_facts_ + i;
                lp_solver_.set_constraint_lower_bound(c_index, -1);
                reset_indices_.push_back(c_index);
            }
        }

        for (std::size_t var = 0; var < num_variables; ++var) {
            const int c_index = ncc_offsets_[var] + state[var];
            lp_solver_.set_constraint_lower_bound(c_index, -1);
            reset_indices_.push_back(c_index);
        }

        lp_solver_.solve();
        EvaluationResult res(true, 0.0);

        if (lp_solver_.has_optimal_solution()) {
            const double v = lp_solver_.get_objective_value();
            res = EvaluationResult(v == 0.0, v);
        }

        // Reset the objective coeffieients
        for (int idx : reset_indices_) {
            lp_solver_.set_constraint_lower_bound(idx, 0.0);
        }

        reset_indices_.clear();

        return res;
    } else {
        const auto goal_explicit = get_goal_explicit();

        for (std::size_t var = 0; var < num_variables; ++var) {
            const int g_val = goal_explicit[var];
            if (g_val == -1) {
                const int idx_state_val = ncc_offsets_[var] + state[var];

                lp_solver_.set_constraint_lower_bound(idx_state_val, -1);
                reset_indices_.push_back(idx_state_val);
            } else if (state[var] != g_val) {
                const int idx_state_val = ncc_offsets_[var] + state[var];
                const int idx_g_val = ncc_offsets_[var] + g_val;

                lp_solver_.set_constraint_lower_bound(idx_state_val, -1);
                lp_solver_.set_constraint_lower_bound(idx_g_val, 1);

                reset_indices_.push_back(idx_state_val);
                reset_indices_.push_back(idx_g_val);
            }
        }

        lp_solver_.solve();
        assert(lp_solver_.has_optimal_solution());
        const double v = lp_solver_.get_objective_value();
        EvaluationResult res(false, v);

        // Reset the objective coeffieients
        for (int idx : reset_indices_) {
            lp_solver_.set_constraint_lower_bound(idx, 0.0);
        }

        reset_indices_.clear();

        return res;
    }
}

void RegroupedOperatorCountingHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    lp::add_lp_solver_option_to_parser(parser);
}

void RegroupedOperatorCountingHeuristic::load_maxprob_lp()
{
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

    num_facts_ = offset + g_variable_domain.back();

    constraints.resize(num_facts_, lp::LPConstraint(0.0, inf));

    /***** Max Prob specific *****/

    // Sink variable?
    lp_vars.emplace_back(0, 1, 1);

    // Set up constraint indices for goal facts

    // Goal variables to corresponding constraint index and goal value
    std::map<int, std::pair<std::size_t, int>> var_to_gconstraint;

    for (const auto& goal_fact : g_goal) {
        const std::size_t cindex = constraints.size();
        auto* constraint = &constraints.emplace_back(0.0, inf);
        constraint->insert(0, -1);
        var_to_gconstraint[goal_fact.first] = {cindex, goal_fact.second};
    }

    /****************************/

    // Set up constraint coefficients of variables Y_{a,e}
    for (const ProbabilisticOperator& op : g_operators) {
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
            lp_vars.emplace_back(0, inf, 0);

            for (const auto& eff : effects) {
                const int var = eff.var;
                const int val = eff.val;

                auto* var_constraints = constraints.data() + ncc_offsets_[var];

                // Always produces / Sometimes produces
                var_constraints[val].insert(lp_var, 1);

                const int pre_val = pre[var];

                assert(pre_val != val && "Redundant effect encountered!");

                if (pre_val != -1) {
                    // Always consumes
                    var_constraints[pre_val].insert(lp_var, -1);
                }

                /***** Max Prob specific *****/

                // Goal constraint
                const auto gconstr_it = var_to_gconstraint.find(var);

                if (gconstr_it != var_to_gconstraint.end()) {
                    const std::size_t cindex = gconstr_it->second.first;
                    const int goal_val = gconstr_it->second.second;
                    if (goal_val == val) {
                        constraints[cindex].insert(lp_var, 1);
                    } else if (pre_val == goal_val) {
                        constraints[cindex].insert(lp_var, -1);
                    }
                }

                /****************************/
            }

            // Skip first variable for regrouping constraints
            if (i == 0) {
                continue;
            }

            // Set up regrouping constraint coefficients
            auto& rg_constraint = constraints.emplace_back(0, 0);
            rg_constraint.insert(first_eff_lpvar, 1.0 / first_eff_probability);
            rg_constraint.insert(lp_var, -1.0 / probability);
        }
    }

    lp_solver_.load_problem(
        lp::LPObjectiveSense::MAXIMIZE,
        lp_vars,
        constraints);

    for (const auto& goal_fact : g_goal) {
        auto& c = constraints[var_to_gconstraint[goal_fact.first].first];
        c.dump();
        constraints[ncc_offsets_[goal_fact.first] + goal_fact.second].dump();
        std::cout << "------" << std::endl;
    }
}

void RegroupedOperatorCountingHeuristic::load_expcost_lp()
{
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

    num_facts_ = offset + g_variable_domain.back();

    constraints.resize(num_facts_, lp::LPConstraint(0.0, inf));

    for (const ProbabilisticOperator& op : g_operators) {
        const int reward = -op.get_cost();

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
            rg_constraint.insert(first_eff_lpvar, 1.0 / first_eff_probability);
            rg_constraint.insert(lp_var, -1.0 / probability);
        }
    }

    lp_solver_.load_problem(
        lp::LPObjectiveSense::MAXIMIZE,
        lp_vars,
        constraints);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "hroc",
    options::parse<GlobalStateEvaluator, RegroupedOperatorCountingHeuristic>);

} // namespace occupation_measure_heuristic
} // namespace probabilistic
