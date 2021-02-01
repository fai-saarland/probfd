#include "occupation_measure_heuristic.h"

#include "../../../global_operator.h"
#include "../../../globals.h"
#include "../../../option_parser.h"
#include "../../../plugin.h"
#include "../../../utils/system.h"
#include "../../../utils/timer.h"
#include "../../analysis_objectives/goal_probability_objective.h"
#include "../../globals.h"
#include "../../probabilistic_operator.h"
#include "../../analysis_objectives/expected_cost_objective.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>

namespace probabilistic {
namespace occupation_measure_heuristic {

void
ProjectionOccupationMeasureHeuristic::generate_hpom_lp(
    lp::LPSolver& lp_solver_,
    std::vector<lp::LPVariable>& lp_vars,
    std::vector<lp::LPConstraint>& constraints,
    std::vector<int>& offset_)
{
    ::verify_no_axioms_no_conditional_effects();
    if (dynamic_cast<GoalProbabilityObjective*>(g_analysis_objective.get())
        == nullptr) {
        std::cerr << "hpom currently only supports MaxProb" << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    const double inf = lp_solver_.get_infinity();

    offset_.resize(g_variable_domain.size(), constraints.size());
    for (unsigned var = 1; var < g_variable_domain.size(); ++var) {
        offset_[var] = offset_[var - 1] + g_variable_domain[var - 1];
    }
    constraints.resize(
        offset_.back() + g_variable_domain.back(), lp::LPConstraint(0, inf));

    lp_vars.emplace_back(0, inf, 1);
    for (unsigned i = 0; i < g_goal.size(); ++i) {
        constraints[offset_[g_goal[i].first] + g_goal[i].second].insert(0, -1);
    }

    std::vector<std::pair<int, int>> tieing_equality;
    std::vector<std::pair<int, int>> tieing_inequality;

    std::vector<int> ref;
    std::vector<int> pre(g_variable_domain.size(), -1);
    std::vector<std::vector<value_type::value_t>> post(
        g_variable_domain.size());
    for (int i = post.size() - 1; i >= 0; --i) {
        post[i].resize(g_variable_domain[i] + 1, 0);
    }
    for (unsigned op_num = 0; op_num < g_operators.size(); ++op_num) {
        const auto& op = g_operators[op_num];
        const auto& pres = op.get_preconditions();
        for (int i = post.size() - 1; i >= 0; --i) {
            std::fill(post[i].begin(), post[i].end(), 0);
            post[i].back() = 1;
        }
        std::fill(pre.begin(), pre.end(), -1);
        for (int i = pres.size() - 1; i >= 0; --i) {
            pre[pres[i].var] = pres[i].val;
        }
        for (int i = op.num_outcomes() - 1; i >= 0; --i) {
            const auto& out = op[i];
            const auto& effs = out.op->get_effects();
            for (int j = effs.size() - 1; j >= 0; --j) {
                ref.push_back(effs[j].var);
                std::vector<value_type::value_t>& probs = post[effs[j].var];
                probs[effs[j].val] += out.prob;
                probs.back() -= out.prob;
            }
        }
        std::sort(ref.begin(), ref.end());
        ref.erase(std::unique(ref.begin(), ref.end()), ref.end());
        for (unsigned i = 0; i < ref.size(); ++i) {
            std::pair<int, int> var_range(lp_vars.size(), 0);
            bool has_pure_self_loop = false;
            const int var = ref[i];
            lp::LPConstraint* flow = &constraints[offset_[var]];
            if (pre[var] == -1) {
                for (int val = 0; val < g_variable_domain[var]; ++val) {
                    if (post[var][val] + post[var].back() >= 1.0) {
                        has_pure_self_loop = true;
                        continue;
                    }
                    const int lpvar = lp_vars.size();
                    lp_vars.emplace_back(0, inf, 0);
                    flow[val].insert(
                        lpvar, -1 + post[var].back() + post[var][val]);
                    for (int val2 = g_variable_domain[var] - 1; val2 >= 0;
                         --val2) {
                        const value_type::value_t prob = post[var][val2];
                        if (val2 != val && prob > 0) {
                            flow[val2].insert(lpvar, prob);
                        }
                    }
                }
            } else {
                const int val = pre[var];
                const int lpvar = lp_vars.size();
                lp_vars.emplace_back(0, inf, 0);
                flow[val].insert(lpvar, -1 + post[var].back() + post[var][val]);
                for (int val2 = g_variable_domain[var] - 1; val2 >= 0; --val2) {
                    const value_type::value_t prob = post[var][val2];
                    if (val2 != val && prob > 0) {
                        flow[val2].insert(lpvar, prob);
                    }
                }
            }
            var_range.second = lp_vars.size();
            if (has_pure_self_loop) {
                tieing_inequality.push_back(var_range);
            } else {
                tieing_equality.push_back(var_range);
            }
        }
        ref.clear();
        if (!tieing_equality.empty()) {
            const auto& base_range = tieing_equality[0];
            for (unsigned j = 1; j < tieing_equality.size(); ++j) {
                constraints.emplace_back(0, 0);
                for (int lpvar = base_range.first; lpvar < base_range.second;
                     ++lpvar) {
                    constraints.back().insert(lpvar, 1);
                }
                for (int lpvar = tieing_equality[j].first;
                     lpvar < tieing_equality[j].second;
                     ++lpvar) {
                    constraints.back().insert(lpvar, -1);
                }
            }
            for (unsigned j = 0; j < tieing_inequality.size(); ++j) {
                constraints.emplace_back(0, inf);
                for (int lpvar = base_range.first; lpvar < base_range.second;
                     ++lpvar) {
                    constraints.back().insert(lpvar, 1);
                }
                for (int lpvar = tieing_inequality[j].first;
                     lpvar < tieing_inequality[j].second;
                     ++lpvar) {
                    constraints.back().insert(lpvar, -1);
                }
            }
        }
    }
}

namespace {
// Explicit goal values (-1 if variable not required)
std::vector<int> get_goal_explicit() {
    std::vector<int> the_goal(g_variable_domain.size(), -1);

    for (const auto& [var, val] : g_goal) {
        the_goal[var] = val;
    }

    return the_goal;
}

// Explicit precondition values (-1 if no precondition for variable)
std::vector<int> get_precondition_explicit(const ProbabilisticOperator& op) {
    std::vector<int> pre(g_variable_domain.size(), -1);

    for (const auto& [var, val] : op.get_preconditions()) {
        pre[var] = val;
    }

    return pre;
}

// Compute an explicit transition probability matrix
std::vector<std::vector<value_type::value_t>>
get_transition_probs_explicit(
    const ProbabilisticOperator& op,
    std::set<int>& ref)
{
    std::vector<std::vector<value_type::value_t>> p(g_variable_domain.size());

    for (size_t i = 0; i < p.size(); ++i) {
        p[i].resize(g_variable_domain[i] + 1, 0.0);
        p[i].back() = 1.0;
    }

    for (const ProbabilisticOutcome &out : op) {
        const auto &effs = out.op->get_effects();
        value_type::value_t prob = out.prob;

        for (int j = (int) effs.size() - 1; j >= 0; --j) {
            int var = effs[j].var;
            int val = effs[j].val;

            ref.insert(var);
            std::vector<value_type::value_t> &probs = p[var];
            probs[val] += prob;
            probs.back() -= prob;
        }
    }

    return p;
}
}

void
ProjectionOccupationMeasureHeuristic::generate_hpom_lp_expcost(
    lp::LPSolver& lp_solver_,
    std::vector<lp::LPVariable>& lp_vars,
    std::vector<lp::LPConstraint>& constraints,
    std::vector<int>& offset_)
{
    assert(lp_vars.empty() && constraints.empty());

    ::verify_no_axioms_no_conditional_effects();

    const double inf = lp_solver_.get_infinity();

    offset_.resize(g_variable_domain.size(), 0);
    for (unsigned var = 1; var < g_variable_domain.size(); ++var) {
        offset_[var] = offset_[var - 1] + g_variable_domain[var - 1];
    }

    // One flow constraint for every state of every atomic projections
    constraints.resize(
        offset_.back() + g_variable_domain.back(),
        lp::LPConstraint(0, 0));

    std::vector<int> the_goal = get_goal_explicit();

    // Build flow contraint coefficients for dummy goal action
    for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
        lp::LPConstraint& goal_constraint = constraints.emplace_back(1, 1);
        lp::LPConstraint* flow = &constraints[offset_[var]];

        if (the_goal[var] == -1) {
            for (int val = 0; val != g_variable_domain[var]; ++val) {
                const int lpvar = lp_vars.size();
                lp_vars.emplace_back(0, inf, 0);
                flow[val].insert(lpvar, 1); // outflow for goal states
                goal_constraint.insert(lpvar, 1); // inflow for dummy goal state
            }
        } else {
            const int val = the_goal[var];
            const int lpvar = lp_vars.size();
            lp_vars.emplace_back(0, inf, 0);
            flow[val].insert(lpvar, 1);
            goal_constraint.insert(lpvar, 1);
        }
    }

    // Now ordinary actions
    for (const ProbabilisticOperator& op : g_operators) {
        const int cost = op.get_cost();
        const std::vector<int> pre = get_precondition_explicit(op);
        std::set<int> affected_vars;
        const std::vector<std::vector<value_type::value_t>> post =
            get_transition_probs_explicit(op, affected_vars);

        // For tying constraints, contains lp variable ranges of projections
        std::vector<std::pair<int, int>> tying;

        // Build flow constraint coefficients...
        for (const int var : affected_vars) {
            std::pair<int, int> var_range(lp_vars.size(), 0);
            lp::LPConstraint* flow = &constraints[offset_[var]];

            const auto& domain = g_variable_domain[var];
            const auto& probs = post[var];

            // Populate flow constraints
            if (pre[var] == -1) {
                for (int val = 0; val < domain; ++val) {
                    const int lpvar = lp_vars.size();
                    lp_vars.emplace_back(0, inf, 0);
                    flow[val].insert(lpvar, 1 - (probs.back() + probs[val]));

                    for (int val2 = 0; val2 < domain; ++val2) {
                        const value_type::value_t prob = probs[val2];
                        if (val2 != val && prob > 0) {
                            flow[val2].insert(lpvar, -prob);
                        }
                    }
                }
            } else {
                const int val = pre[var];
                const int lpvar = lp_vars.size();
                lp_vars.emplace_back(0, inf, 0);
                flow[val].insert(lpvar, 1 - (probs.back() + probs[val]));

                for (int val2 = 0; val2 < domain; ++val2) {
                    const value_type::value_t prob = probs[val2];
                    if (val2 != val && prob > 0) {
                        flow[val2].insert(lpvar, -prob);
                    }
                }
            }

            var_range.second = lp_vars.size();
            tying.push_back(var_range);
        }

        // Build tying constraints, tie everything to first projection
        if (!tying.empty()) {
            const auto& [base_range_b, base_range_e] = tying[0];
            for (unsigned j = 1; j < tying.size(); ++j) {
                const auto& [j_range_b, j_range_e] = tying[j];

                lp::LPConstraint &tying_constraint =
                    constraints.emplace_back(0, 0);

                for (int lpvar = base_range_b; lpvar < base_range_e; ++lpvar) {
                    tying_constraint.insert(lpvar, 1);
                }

                for (int lpvar = j_range_b; lpvar < j_range_e; ++lpvar) {
                    tying_constraint.insert(lpvar, -1);
                }
            }
        }

        // Set objective coefficients for occupation measures of the first
        // projection (does not matter which one)
        const auto& [range_b, range_e] = tying[0];
        for (int i = range_b; i < range_e; ++i) {
            lp_vars[i].objective_coefficient = cost;
        }
    }
}

ProjectionOccupationMeasureHeuristic::ProjectionOccupationMeasureHeuristic(
    const options::Options& opts)
    : lp_solver_(lp::LPSolverType(opts.get_enum("lpsolver")))
{
    std::cout << "Initializing projection occupation measure heuristic ..."
              << std::endl;
    utils::Timer timer;

    std::vector<lp::LPVariable> lp_vars;
    std::vector<lp::LPConstraint> constraints;

    if (dynamic_cast<GoalProbabilityObjective*>(g_analysis_objective.get())) {
        generate_hpom_lp(lp_solver_, lp_vars, constraints, offset_);
        lp_solver_.load_problem(
            lp::LPObjectiveSense::MAXIMIZE, lp_vars, constraints);
    } else {
        assert(
            dynamic_cast<ExpectedCostObjective*>(g_analysis_objective.get()));

        generate_hpom_lp_expcost(lp_solver_, lp_vars, constraints, offset_);
        lp_solver_.load_problem(
            lp::LPObjectiveSense::MINIMIZE, lp_vars, constraints);
    }

    std::cout << "Finished POM LP setup after " << timer << std::endl;
}

EvaluationResult
ProjectionOccupationMeasureHeuristic::evaluate(const GlobalState& state)
{
    if (dynamic_cast<GoalProbabilityObjective*>(g_analysis_objective.get())) {
        for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
            lp_solver_.set_constraint_lower_bound(
                offset_[var] + state[var], -1);
        }

        lp_solver_.solve();

        EvaluationResult res(true, 0.0);
        if (lp_solver_.has_optimal_solution()) {
            const double v = lp_solver_.get_objective_value();
            res = EvaluationResult(v == 0.0, v);
        }

        for (unsigned i = 0; i < g_goal.size(); ++i) {
            const auto&[g_var, g_val] = g_goal[i];
            if (state[g_var] == g_val) {
                lp_solver_.set_constraint_lower_bound((int) i, 0);
            }
        }

        for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
            lp_solver_.set_constraint_lower_bound(offset_[var] + state[var], 0);
        }

        return res;
    } else {
        assert(
            dynamic_cast<ExpectedCostObjective*>(g_analysis_objective.get()));

        // Set to initial state in LP
        for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
            lp_solver_.set_constraint_bounds(offset_[var] + state[var], 1, 1);
        }

        lp_solver_.solve();
        assert (lp_solver_.has_optimal_solution());

        // Costs are negative rewards, return negative solution.
        const double v = -lp_solver_.get_objective_value();
        EvaluationResult res = EvaluationResult(false, v);

        // Undo for next evaluate
        for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
            lp_solver_.set_constraint_bounds(offset_[var] + state[var], 0, 0);
        }

        return res;
    }
}

void
ProjectionOccupationMeasureHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    lp::add_lp_solver_option_to_parser(parser);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "hpom",
    options::parse<GlobalStateEvaluator, ProjectionOccupationMeasureHeuristic>);

} // namespace occupation_measure_heuristic
} // namespace probabilistic

