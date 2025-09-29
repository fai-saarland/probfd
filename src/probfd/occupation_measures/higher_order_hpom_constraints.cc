#include "probfd/occupation_measures/higher_order_hpom_constraints.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/cost_function.h"

#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include "downward/lp/lp_solver.h"

#include "downward/task_utils/task_properties.h"
#include "probfd/probabilistic_operator_space.h"
#include "probfd/termination_costs.h"

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <print>
#include <utility>

using namespace downward;

namespace probfd::occupation_measures {

static bool next_pattern(std::size_t num_variables, std::vector<int>& pattern)
{
    bool overflow = false;

    int idx;
    int base = 0;

    int high = static_cast<int>(num_variables);

    for (idx = static_cast<int>(pattern.size()) - 1; idx >= 0; --idx, --high) {
        const int next = pattern[idx] + 1;

        if (next < high) {
            pattern[idx] = next;
            base = next + 1;
            break;
        }
    }

    if (idx == -1) { overflow = true; }

    for (++idx; idx < static_cast<int>(pattern.size()); ++idx) {
        pattern[idx] = base++;
    }

    return !overflow;
}

static std::vector<int> get_first_partial_state(
    const std::vector<int>& pattern,
    const std::vector<int>& pindices)
{
    std::vector<int> buffer(pattern.size());

    for (size_t i = 0; i != pattern.size(); ++i) {
        const int val = pindices[pattern[i]];
        buffer[i] = val != -1 ? val : 0;
    }

    return buffer;
}

static bool next_partial_state(
    const VariableSpace& variables,
    std::vector<int>& pstate,
    const std::vector<int>& pattern,
    const std::vector<int>& pindices)
{
    for (int idx = static_cast<int>(pattern.size()) - 1; idx >= 0; --idx) {
        const int var = pattern[idx];

        if (pindices[var] != -1) continue;

        const int next = pstate[idx] + 1;

        if (next < variables[var].get_domain_size()) {
            pstate[idx] = next;
            return true;
        }

        pstate[idx] = 0;
    }

    return false;
}

std::vector<int> HigherOrderHPOMGenerator::get_first_pattern() const
{
    std::vector<int> pattern(projection_size_);

    for (size_t i = 0; i != pattern.size(); ++i) {
        pattern[i] = static_cast<int>(i);
    }

    return pattern;
}

int HigherOrderHPOMGenerator::PatternInfo::get_state_id(
    const std::vector<int>& state) const
{
    int res = 0;

    for (size_t i = 0; i < multipliers.size(); ++i) {
        res += multipliers[i] * state[i];
    }

    return res;
}

int HigherOrderHPOMGenerator::PatternInfo::get_updated_id(
    const std::vector<int>& pattern,
    const std::vector<int>& state,
    const std::vector<int>& pstate) const
{
    if (pstate.empty()) { return get_state_id(state); }

    int res = 0;

    for (size_t i = 0; i < pattern.size(); ++i) {
        const int val = pstate[pattern[i]];
        res += multipliers[i] * (val == -1 ? state[i] : val);
    }

    return res;
}

int HigherOrderHPOMGenerator::PatternInfo::to_id(
    const std::vector<int>& pattern,
    const State& state) const
{
    int id = 0;

    for (size_t i = 0; i != pattern.size(); ++i) {
        id += multipliers[i] * state[pattern[i]];
    }

    return id;
}

HigherOrderHPOMGenerator::HigherOrderHPOMGenerator(int projection_size)
    : projection_size_(projection_size)
{
}

void HigherOrderHPOMGenerator::initialize_constraints(
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
            "currently unsupported in higher-order hpom implementation.");
    }

    const bool maxprob = term_costs.get_non_goal_termination_cost() == 1_vt;

    const std::size_t num_variables = variables.size();

    ::task_properties::verify_no_axioms(axioms);
    task_properties::verify_no_conditional_effects(operators);
    std::println(std::cout, "Initializing HO-HPOM LP constraints...");

    utils::Timer timer;

    const double inf = lp.get_infinity();

    // Prepare fact variable offsets
    infos_.reserve(num_variables);

    int offset = 0;

    {
        std::vector<int> pattern = get_first_pattern();
        do {
            auto& info = infos_.emplace_back(offset);

            int multiplier = 1;

            for (int var_id : pattern) {
                info.multipliers.push_back(multiplier);
                multiplier *= variables[var_id].get_domain_size();
            }

            offset += multiplier;
        } while (next_pattern(num_variables, pattern));
    }

    auto& lp_variables = lp.get_variables();
    auto& lp_constraints = lp.get_constraints();

    // One flow constraint for every state of every projection
    lp_constraints.resize(offset, lp::LPConstraint(-inf, 0.0_vt));

    // Variable representing total inflow to artificial goal
    // Maximized in MaxProb, must be constant 1 for SSPs
    lp_variables.emplace_back(maxprob ? 0 : 1, 1, maxprob ? -1 : 0);

    std::vector<int> the_goal(num_variables, -1);

    for (const auto [var, value] : goals) { the_goal[var] = value; }

    // Build flow contraint coefficients for dummy goal action
    {
        std::vector<int> pattern = get_first_pattern();
        int pattern_id = 0;
        do {
            const PatternInfo& info = infos_[pattern_id++];

            lp::LPConstraint& goal_constraint =
                lp_constraints.emplace_back(0, 0);
            goal_constraint.insert(0, -1);
            lp::LPConstraint* flow = &lp_constraints[0] + info.offset;

            std::vector<int> pstate =
                get_first_partial_state(pattern, the_goal);
            do {
                const int lpvar = lp_variables.size();
                lp_variables.emplace_back(0, inf, 0);

                // outflow for goal states
                flow[info.get_state_id(pstate)].insert(lpvar, 1);

                // inflow for dummy goal state
                goal_constraint.insert(lpvar, 1);
            } while (next_partial_state(variables, pstate, pattern, the_goal));
        } while (next_pattern(num_variables, pattern));
    }

    // Now ordinary actions
    for (const ProbabilisticOperatorProxy op : operators) {
        const auto cost =
            maxprob ? 0 : cost_function.get_operator_cost(op.get_id());

        // Get dense precondition
        std::vector<int> pre(num_variables, -1);

        for (const auto [var, value] : op.get_preconditions()) {
            pre[var] = value;
        }

        // For tying constraints, contains lp variable ranges of projections
        std::vector<std::pair<int, int>> tieing_equality;

        // Build flow constraint coefficients...
        std::vector<int> pattern = get_first_pattern();
        int pattern_id = 0;
        do {
            const PatternInfo& info = infos_[pattern_id++];

            const int first_lpvar = lp_variables.size();
            lp::LPConstraint* flow = &lp_constraints[0] + info.offset;

            std::vector<int> pstate = get_first_partial_state(pattern, pre);
            do {
                int astate_id = info.get_state_id(pstate);

                // Occupation measure / flow variable x_{d, a}
                const int lpvar = lp_variables.size();
                lp_variables.emplace_back(0, inf, 0);

                std::map<int, value_t> transitions;
                transitions.emplace(astate_id, 1.0);

                for (const ProbabilisticOutcomeProxy& outcome :
                     op.get_outcomes()) {
                    const auto probability = outcome.get_probability();

                    std::vector<int> effects(num_variables, -1);

                    for (const auto effect_proxy : outcome.get_effects()) {
                        const auto& [eff_var, eff_val] =
                            effect_proxy.get_fact();
                        effects[eff_var] = eff_val;
                    }

                    int id = info.get_updated_id(pattern, pstate, effects);

                    if (astate_id == id) {
                        transitions[id] -= probability;
                        continue;
                    }

                    auto [it, inserted] = transitions.emplace(id, -probability);

                    if (!inserted) { it->second -= probability; }
                }

                // Pure self loop check
                if (transitions.size() == 1) { continue; }

                for (const auto& [succ_id, prob] : transitions) {
                    assert(succ_id == astate_id || prob < 0.0_vt);
                    flow[succ_id].insert(lpvar, prob);
                }
            } while (next_partial_state(variables, pstate, pattern, pre));

            tieing_equality.emplace_back(first_lpvar, lp_variables.size());
        } while (next_pattern(num_variables, pattern));

        // Build tying constraints, tie everything to first projection
        if (!tieing_equality.empty()) {
            const auto& base_range = tieing_equality[0];

            // Set objective coefficients for occ. measures of first
            // projection
            for (int i = base_range.first; i < base_range.second; ++i) {
                lp_variables[i].objective_coefficient = cost;
            }

            for (std::size_t j = 1; j < tieing_equality.size(); ++j) {
                const auto& tieing_eq = tieing_equality[j];

                auto& tieing_constraint = lp_constraints.emplace_back(0, 0);

                for (int i = base_range.first; i < base_range.second; ++i) {
                    tieing_constraint.insert(i, 1);
                }

                for (int i = tieing_eq.first; i < tieing_eq.second; ++i) {
                    tieing_constraint.insert(i, -1);
                }
            }
        }
    }

    std::println(std::cout, "Finished HO-POM LP setup after {}", timer());
}

void HigherOrderHPOMGenerator::update_constraints(
    const State& state,
    lp::LPSolver& lp_solver)
{
    // Set to initial state in LP
    std::vector<int> pattern = get_first_pattern();
    int pattern_id = 0;

    do {
        const PatternInfo& info = infos_[pattern_id++];
        const int index = info.offset + info.to_id(pattern, state);
        lp_solver.set_constraint_upper_bound(index, 1.0);
    } while (next_pattern(state.size(), pattern));
}

void HigherOrderHPOMGenerator::reset_constraints(
    const State& state,
    lp::LPSolver& lp_solver)
{
    // Unset to initial state in LP
    std::vector<int> pattern = get_first_pattern();
    int pattern_id = 0;

    do {
        const PatternInfo& info = infos_[pattern_id++];
        const int index = info.offset + info.to_id(pattern, state);
        lp_solver.set_constraint_upper_bound(index, 0.0);
    } while (next_pattern(state.size(), pattern));
}

HigherOrderHPOMGeneratorFactory::HigherOrderHPOMGeneratorFactory(
    int projection_size)
    : projection_size_(projection_size)
{
}

std::unique_ptr<ConstraintGenerator>
HigherOrderHPOMGeneratorFactory::construct_constraint_generator(
    const SharedProbabilisticTask&)
{
    return std::make_unique<HigherOrderHPOMGenerator>(projection_size_);
}

} // namespace probfd::occupation_measures
