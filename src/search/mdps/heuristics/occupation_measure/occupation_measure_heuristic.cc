#include "occupation_measure_heuristic.h"

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
#include <iostream>
#include <memory>

namespace probabilistic {
namespace occupation_measure_heuristic {

ProjectionOccupationMeasureHeuristic::ProjectionOccupationMeasureHeuristic(
    const options::Options& opts)
    : lp_solver_(lp::LPSolverType(opts.get_enum("lpsolver")))
{
    ::verify_no_axioms_no_conditional_effects();
    if (dynamic_cast<GoalProbabilityObjective*>(g_analysis_objective.get())
        == nullptr) {
        std::cerr << "hpom currently only supports MaxProb" << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    std::cout << "Initializing projection occupation measure heuristic ..."
              << std::endl;
    utils::Timer timer;

    const double inf = lp_solver_.get_infinity();
    std::vector<lp::LPVariable> lp_vars;
    std::vector<lp::LPConstraint> constraints;

    offset_.resize(g_variable_domain.size(), g_goal.size());
    for (unsigned var = 1; var < g_variable_domain.size(); ++var) {
        offset_[var] = offset_[var - 1] + g_variable_domain[var - 1];
    }
    constraints.resize(
        g_goal.size() + offset_.back() + g_variable_domain.back(),
        lp::LPConstraint(0, inf));

    lp_vars.emplace_back(0, 1, 1);
    goal_.resize(g_variable_domain.size(), -1);
    for (unsigned i = 0; i < g_goal.size(); ++i) {
        goal_[g_goal[i].first] = i;
        constraints[i].insert(0, -1);
    }

    std::vector<std::pair<int, int>> tieing;

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
            tieing.emplace_back(lp_vars.size(), 0);
            bool has_pure_self_loop = false;
            const int var = ref[i];
            const int goal_val =
                goal_[var] >= 0 ? g_goal[goal_[var]].second : -1;
            lp::LPConstraint* flow = &constraints[offset_[var]];
            if (pre[var] == -1) {
                for (int val = 0; val < g_variable_domain[var]; ++val) {
                    if (post[var][val] + post[var].back() >= 1.0) {
                        has_pure_self_loop = true;
                        continue;
                    }
                    const int lpvar = lp_vars.size();
                    lp_vars.emplace_back(0, inf, 0);
                    if (goal_val >= 0) {
                        value_type::value_t gprob = 0;
                        if (goal_val == val) {
                            gprob = post[var][val] + post[var].back() - 1;
                        } else {
                            gprob = post[var][goal_val];
                        }
                        if (gprob != 0) {
                            constraints[goal_[var]].insert(lpvar, gprob);
                        }
                    }
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
                if (goal_val >= 0) {
                    value_type::value_t gprob = 0;
                    if (goal_val == val) {
                        gprob = post[var][val] + post[var].back() - 1;
                    } else {
                        gprob = post[var][goal_val];
                    }
                    if (gprob != 0) {
                        constraints[goal_[var]].insert(lpvar, gprob);
                    }
                }
                flow[val].insert(lpvar, -1 + post[var].back() + post[var][val]);
                for (int val2 = g_variable_domain[var] - 1; val2 >= 0; --val2) {
                    const value_type::value_t prob = post[var][val2];
                    if (val2 != val && prob > 0) {
                        flow[val2].insert(lpvar, prob);
                    }
                }
            }
            tieing.back().second = lp_vars.size();
            if (has_pure_self_loop) {
                tieing.pop_back();
            }
        }
        ref.clear();
        int i = 0;
        for (unsigned j = i + 1; j < tieing.size(); ++j) {
            constraints.emplace_back(0, 0);
            for (int lpvar = tieing[i].first; lpvar < tieing[i].second;
                 ++lpvar) {
                constraints.back().insert(lpvar, 1);
            }
            for (int lpvar = tieing[j].first; lpvar < tieing[j].second;
                 ++lpvar) {
                constraints.back().insert(lpvar, -1);
            }
        }
        tieing.clear();
    }
    lp_solver_.load_problem(
        lp::LPObjectiveSense::MAXIMIZE, lp_vars, constraints);

    std::cout << "Finished POM LP setup after " << timer << std::endl;
}

EvaluationResult
ProjectionOccupationMeasureHeuristic::evaluate(const GlobalState& state)
{
    for (unsigned i = 0; i < g_goal.size(); ++i) {
        if (state[g_goal[i].first] == g_goal[i].second) {
            lp_solver_.set_constraint_lower_bound(i, -1);
        }
    }
    for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
        lp_solver_.set_constraint_lower_bound(offset_[var] + state[var], -1);
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
        lp_solver_.set_constraint_lower_bound(offset_[var] + state[var], 0);
    }
    return res;
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

