#include "regrouped_operator_counting_heuristic.h"

#include "../../../global_operator.h"
#include "../../../globals.h"
#include "../../../option_parser.h"
#include "../../../plugin.h"
#include "../../../utils/system.h"
#include "../../../utils/timer.h"
#include "../../globals.h"
#include "../../goal_probability_objective.h"
#include "../../probabilistic_operator.h"

#include <algorithm>
#include <cassert>
#include <memory>

namespace probabilistic {
namespace occupation_measure_heuristic {

RegroupedOperatorCountingHeuristic::RegroupedOperatorCountingHeuristic(
    const options::Options& opts)
    : lp_solver_(lp::LPSolverType(opts.get_enum("lpsolver")))
{
    ::verify_no_axioms_no_conditional_effects();
    if (dynamic_cast<GoalProbabilityObjective*>(g_analysis_objective.get())
        == nullptr) {
        std::cerr << "hroc currently only supports MaxProb" << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    std::cout << "Initializing regrouped operator counting heuristic..."
              << std::endl;
    utils::Timer timer;

    const double inf = lp_solver_.get_infinity();
    std::vector<lp::LPVariable> lp_vars;
    std::vector<lp::LPConstraint> constraints;
    constraint_offsets_.resize(g_variable_domain.size(), g_goal.size());
    for (unsigned var = 1; var < g_variable_domain.size(); ++var) {
        constraint_offsets_[var] =
            constraint_offsets_[var - 1] + g_variable_domain[var - 1];
    }
    constraints.resize(
        g_goal.size() + constraint_offsets_.back() + g_variable_domain.back(),
        lp::LPConstraint(0, inf));
    lp_vars.emplace_back(0, 1, 1);

    std::vector<int> goal(g_variable_domain.size(), -1);
    for (unsigned i = 0; i < g_goal.size(); ++i) {
        goal[g_goal[i].first] = i;
        constraints[i].insert(0, -1);
    }

    std::vector<int> pre(g_variable_domain.size());
    for (unsigned op_num = 0; op_num < g_operators.size(); ++op_num) {
        const ProbabilisticOperator& op = g_operators[op_num];
        int lp_var_start = lp_vars.size();

        std::fill(pre.begin(), pre.end(), -1);
        for (unsigned i = 0; i < op.get_preconditions().size(); ++i) {
            const auto& f = op.get_preconditions()[i];
            pre[f.var] = f.val;
        }

        for (unsigned out_num = 0; out_num < op.num_outcomes(); ++out_num) {
            const GlobalOperator& outcome = *(op[out_num].op);
            const int lp_var = lp_vars.size();
            lp_vars.emplace_back(0, inf, 0);
            for (unsigned i = 0; i < outcome.get_effects().size(); ++i) {
                const auto& eff = outcome.get_effects()[i];
                constraints[constraint_offsets_[eff.var] + eff.val].insert(
                    lp_var, 1);
                if (pre[eff.var] != -1) {
                    constraints[constraint_offsets_[eff.var] + pre[eff.var]]
                        .insert(lp_var, -1);
                }
                if (goal[eff.var] != -1) {
                    const int goal_val = g_goal[goal[eff.var]].second;
                    if (goal_val == eff.val) {
                        constraints[goal[eff.var]].insert(lp_var, 1);
                    } else if (pre[eff.var] == goal_val) {
                        constraints[goal[eff.var]].insert(lp_var, -1);
                    }
                }
            }
        }

        for (int i = op.num_outcomes() - 1, lp_var = lp_vars.size() - 1;
             lp_var > lp_var_start;
             --lp_var, --i) {
            constraints.emplace_back(0, 0);
            constraints.back().insert(lp_var_start, op[0].prob);
            constraints.back().insert(lp_var, -op[i].prob);
        }
    }

    lp_solver_.load_problem(
        lp::LPObjectiveSense::MAXIMIZE, lp_vars, constraints);

    std::cout << "Finished ROC LP setup after " << timer << std::endl;
}

EvaluationResult
RegroupedOperatorCountingHeuristic::evaluate(const GlobalState& state)
{
    for (unsigned i = 0; i < g_goal.size(); ++i) {
        if (state[g_goal[i].first] == g_goal[i].second) {
            lp_solver_.set_constraint_lower_bound(i, -1);
        }
    }
    for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
        lp_solver_.set_constraint_lower_bound(
            constraint_offsets_[var] + state[var], -1);
    }
    lp_solver_.solve();
    EvaluationResult res(true, 0.0);
    if (lp_solver_.has_optimal_solution()) {
        res = EvaluationResult(false, lp_solver_.get_objective_value());
    }
    for (unsigned i = 0; i < g_goal.size(); ++i) {
        if (state[g_goal[i].first] == g_goal[i].second) {
            lp_solver_.set_constraint_lower_bound(i, 0);
        }
    }
    for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
        lp_solver_.set_constraint_lower_bound(
            constraint_offsets_[var] + state[var], 0);
    }
    return res;
}

void
RegroupedOperatorCountingHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    lp::add_lp_solver_option_to_parser(parser);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "hroc",
    options::parse<GlobalStateEvaluator, RegroupedOperatorCountingHeuristic>);

} // namespace occupation_measure_heuristic
} // namespace probabilistic

