#include "pdbs/pattern_database.h"

#include "pdbs/match_tree.h"

#include "algorithms/priority_queues.h"

#include "utils/collections.h"
#include "utils/logging.h"
#include "utils/math.h"
#include "utils/rng.h"
#include "utils/timer.h"

#include "global_operator.h"
#include "global_state.h"
#include "globals.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

namespace pdbs {
AbstractOperator::AbstractOperator(
    const vector<FactPair>& prev_pairs,
    const vector<FactPair>& pre_pairs,
    const vector<FactPair>& eff_pairs,
    int cost,
    const vector<size_t>& hash_multipliers,
    int concrete_op_id)
    : concrete_op_id(concrete_op_id)
    , cost(cost)
    , regression_preconditions(prev_pairs)
{
    regression_preconditions.insert(
        regression_preconditions.end(), eff_pairs.begin(), eff_pairs.end());
    // Sort preconditions for MatchTree construction.
    sort(regression_preconditions.begin(), regression_preconditions.end());
    for (size_t i = 1; i < regression_preconditions.size(); ++i) {
        assert(
            regression_preconditions[i].var
            != regression_preconditions[i - 1].var);
    }
    hash_effect = 0;
    assert(pre_pairs.size() == eff_pairs.size());
    for (size_t i = 0; i < pre_pairs.size(); ++i) {
        int var = pre_pairs[i].var;
        assert(var == eff_pairs[i].var);
        int old_val = eff_pairs[i].value;
        int new_val = pre_pairs[i].value;
        assert(new_val != -1);
        size_t effect = (new_val - old_val) * hash_multipliers[var];
        hash_effect += effect;
    }
}

AbstractOperator::~AbstractOperator() { }

void
AbstractOperator::dump(const Pattern& pattern) const
{
    cout << "AbstractOperator:" << endl;
    cout << "Regression preconditions:" << endl;
    for (size_t i = 0; i < regression_preconditions.size(); ++i) {
        int var_id = regression_preconditions[i].var;
        int val = regression_preconditions[i].value;
        cout << "Variable: " << var_id
             << " (True name: " << g_fact_names[pattern[var_id]][val]
             << ", Index: " << i << ") Value: " << val << endl;
    }
    cout << "Hash effect:" << hash_effect << endl;
}

PatternDatabase::PatternDatabase(
    const Pattern& pattern,
    OperatorCost operator_cost,
    bool dump,
    bool compute_plan,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool compute_extended_plan)
    : PatternDatabase(
          pattern,
          dump,
          [](OperatorCost cost_type) {
              if (cost_type == NORMAL) {
                  return std::vector<int>();
              }
              std::vector<int> res(g_operators.size(), 0);
              for (int i = g_operators.size() - 1; i >= 0; i--) {
                  res[i] =
                      ::get_adjusted_action_cost(g_operators[i], cost_type);
              }
              return res;
          }(operator_cost),
          compute_plan,
          rng,
          compute_extended_plan)
{
}

PatternDatabase::PatternDatabase(
    const Pattern& pattern,
    bool dump,
    const vector<int>& operator_costs,
    bool compute_plan,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool compute_extended_plan)
    : pattern(pattern)
{
    ::verify_no_axioms();
    ::verify_no_conditional_effects();
    assert(
        operator_costs.empty() || operator_costs.size() == g_operators.size());
    assert(utils::is_sorted_unique(pattern));

    utils::Timer timer;
    hash_multipliers.reserve(pattern.size());
    num_states = 1;
    for (int pattern_var_id : pattern) {
        hash_multipliers.push_back(num_states);
        int domain_size = g_variable_domain[pattern_var_id];
        if (utils::is_product_within_limit(
                num_states, domain_size, numeric_limits<int>::max())) {
            num_states *= domain_size;
        } else {
            cerr << "Given pattern is too large! (Overflow occured): " << endl;
            cerr << pattern << endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }
    create_pdb(operator_costs, compute_plan, rng, compute_extended_plan);
    if (dump)
        cout << "PDB construction time: " << timer << endl;
}

void PatternDatabase::multiply_out(
    const vector<int>& variable_to_index,
    int pos,
    int cost,
    vector<FactPair>& prev_pairs,
    vector<FactPair>& pre_pairs,
    vector<FactPair>& eff_pairs,
    const vector<FactPair>& effects_without_pre,
    int concrete_op_id,
    vector<AbstractOperator>& operators)
{
    if (pos == static_cast<int>(effects_without_pre.size())) {
        // All effects without precondition have been checked: insert op.
        if (!eff_pairs.empty()) {
            operators.push_back(AbstractOperator(
                prev_pairs,
                pre_pairs,
                eff_pairs,
                cost,
                hash_multipliers,
                concrete_op_id));
        }
    } else {
        // For each possible value for the current variable, build an
        // abstract operator.
        int var_id = effects_without_pre[pos].var;
        int local_id = variable_to_index[var_id];
        int eff = effects_without_pre[pos].value;
        for (int i = 0; i < g_variable_domain[var_id]; ++i) {
            if (i != eff) {
                pre_pairs.emplace_back(local_id, i);
                eff_pairs.emplace_back(local_id, eff);
            } else {
                prev_pairs.emplace_back(local_id, i);
            }
            multiply_out(
                variable_to_index,
                pos + 1,
                cost,
                prev_pairs,
                pre_pairs,
                eff_pairs,
                effects_without_pre,
                concrete_op_id,
                operators);
            if (i != eff) {
                pre_pairs.pop_back();
                eff_pairs.pop_back();
            } else {
                prev_pairs.pop_back();
            }
        }
    }
}

void
PatternDatabase::build_abstract_operators(
    const GlobalOperator& op,
    int cost,
    const vector<int>& variable_to_index,
    vector<AbstractOperator>& operators)
{
    // All variable value pairs that are a prevail condition
    vector<FactPair> prev_pairs;
    // All variable value pairs that are a precondition (value != -1)
    vector<FactPair> pre_pairs;
    // All variable value pairs that are an effect
    vector<FactPair> eff_pairs;
    // All variable value pairs that are a precondition (value = -1)
    vector<FactPair> effects_without_pre;

    size_t num_vars = g_variable_domain.size();
    vector<bool> has_precond_and_effect_on_var(num_vars, false);
    vector<bool> has_precondition_on_var(num_vars, false);

    for (const auto& pre : op.get_preconditions())
        has_precondition_on_var[pre.var] = true;

    for (const auto& eff : op.get_effects()) {
        int var_id = eff.var;
        int pattern_var_id = variable_to_index[var_id];
        int val = eff.val;
        if (pattern_var_id != -1) {
            if (has_precondition_on_var[var_id]) {
                has_precond_and_effect_on_var[var_id] = true;
                eff_pairs.emplace_back(pattern_var_id, val);
            } else {
                effects_without_pre.emplace_back(var_id, val);
            }
        }
    }
    for (const auto& pre : op.get_preconditions()) {
        int var_id = pre.var;
        int pattern_var_id = variable_to_index[var_id];
        int val = pre.val;
        if (pattern_var_id != -1) { // variable occurs in pattern
            if (has_precond_and_effect_on_var[var_id]) {
                pre_pairs.emplace_back(pattern_var_id, val);
            } else {
                prev_pairs.emplace_back(pattern_var_id, val);
            }
        }
    }
    multiply_out(
        variable_to_index,
        0,
        cost,
        prev_pairs,
        pre_pairs,
        eff_pairs,
        effects_without_pre,
        op.get_id(),
        operators);
}

void PatternDatabase::create_pdb(
    const vector<int>& operator_costs,
    bool compute_plan,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool compute_extended_plan)
{
    vector<int> variable_to_index(g_variable_domain.size(), -1);
    for (size_t i = 0; i < pattern.size(); ++i) {
        variable_to_index[pattern[i]] = i;
    }

    // compute all abstract operators
    vector<AbstractOperator> operators;
    for (unsigned op_num = 0; op_num < g_operators.size(); op_num++) {
        const GlobalOperator& op = g_operators[op_num];
        int op_cost;
        if (operator_costs.empty()) {
            op_cost = op.get_cost();
        } else {
            op_cost = operator_costs[op_num];
        }
        build_abstract_operators(op, op_cost, variable_to_index, operators);
    }

    // build the match tree
    MatchTree match_tree(pattern, hash_multipliers);
    for (size_t op_id = 0; op_id < operators.size(); ++op_id) {
        const AbstractOperator& op = operators[op_id];
        match_tree.insert(op_id, op.get_regression_preconditions());
    }

    // compute abstract goal var-val pairs
    vector<FactPair> abstract_goals;
    for (unsigned i = 0; i < g_goal.size(); i++) {
        int var_id = g_goal[i].first;
        int val = g_goal[i].second;
        if (variable_to_index[var_id] != -1) {
            abstract_goals.emplace_back(variable_to_index[var_id], val);
        }
    }

    distances.reserve(num_states);
    // first implicit entry: priority, second entry: index for an abstract state
    priority_queues::AdaptiveQueue<size_t> pq;

    // initialize queue
    for (size_t state_index = 0; state_index < num_states; ++state_index) {
        if (is_goal_state(state_index, abstract_goals)) {
            pq.push(0, state_index);
            distances.push_back(0);
        } else {
            distances.push_back(numeric_limits<int>::max());
        }
    }

    std::vector<int> generating_op_ids;

    if (compute_plan) {
        /*
          If computing a plan during Dijkstra, we store, for each state,
          an operator leading from that state to another state on a
          strongly optimal plan of the PDB. We store the first operator
          encountered during Dijkstra and only update it if the goal distance
          of the state was updated. Note that in the presence of zero-cost
          operators, this does not guarantee that we compute a strongly
          optimal plan because we do not minimize the number of used zero-cost
          operators.
         */
        generating_op_ids.resize(num_states);
    }

    // Dijkstra loop
    while (!pq.empty()) {
        pair<int, size_t> node = pq.pop();
        int distance = node.first;
        size_t state_index = node.second;
        if (distance > distances[state_index]) {
            continue;
        }

        // regress abstract_state
        vector<int> applicable_operator_ids;
        match_tree.get_applicable_operator_ids(
            state_index, applicable_operator_ids);
        for (int op_id : applicable_operator_ids) {
            const AbstractOperator& op = operators[op_id];
            size_t predecessor = state_index + op.get_hash_effect();
            int alternative_cost = distances[state_index] + op.get_cost();
            if (alternative_cost < distances[predecessor]) {
                distances[predecessor] = alternative_cost;
                pq.push(alternative_cost, predecessor);
                if (compute_plan) {
                    generating_op_ids[predecessor] = op_id;
                }
            }
        }
    }

    // Compute abstract plan
    if (compute_plan) {
        /*
          Using the generating operators computed during Dijkstra, we start
          from the initial state and follow the generating operator to the
          next state. Then we compute all operators of the same cost inducing
          the same abstract transition and randomly pick one of them to
          set for the next state. We iterate until reaching a goal state.
          Note that this kind of plan extraction does not uniformly at random
          consider all successor of a state but rather uses the arbitrarily
          chosen generating operator to settle on one successor state, which
          is biased by the number of operators leading to the same successor
          from the given state.
        */
        GlobalState initial_state = g_initial_state();
        int current_state = hash_index(initial_state);
        if (distances[current_state] != numeric_limits<int>::max()) {
            while (!is_goal_state(current_state, abstract_goals)) {
                int op_id = generating_op_ids[current_state];
                assert(op_id != -1);
                const AbstractOperator& op = operators[op_id];
                int successor_state = current_state - op.get_hash_effect();

                // Compute equivalent ops
                vector<int> cheapest_operators;
                vector<int> applicable_operator_ids;
                match_tree.get_applicable_operator_ids(
                    successor_state,
                    applicable_operator_ids);
                for (int applicable_op_id : applicable_operator_ids) {
                    const AbstractOperator& applicable_op =
                        operators[applicable_op_id];
                    int predecessor =
                        successor_state + applicable_op.get_hash_effect();
                    if (predecessor == current_state &&
                        op.get_cost() == applicable_op.get_cost()) {
                        cheapest_operators.emplace_back(
                            applicable_op.get_concrete_op_id());
                    }
                }
                if (compute_extended_plan) {
                    rng->shuffle(cheapest_operators);
                    wildcard_plan.push_back(move(cheapest_operators));
                } else {
                    int random_op_id = *rng->choose(cheapest_operators);
                    wildcard_plan.emplace_back();
                    wildcard_plan.back().push_back(random_op_id);
                }

                current_state = successor_state;
            }
        }
    }
}

bool
PatternDatabase::is_goal_state(
    const size_t state_index,
    const vector<FactPair>& abstract_goals) const
{
    for (const FactPair& abstract_goal : abstract_goals) {
        int pattern_var_id = abstract_goal.var;
        int var_id = pattern[pattern_var_id];
        int temp = state_index / hash_multipliers[pattern_var_id];
        int val = temp % g_variable_domain[var_id];
        if (val != abstract_goal.value) {
            return false;
        }
    }
    return true;
}

size_t
PatternDatabase::hash_index(const GlobalState& state) const
{
    size_t index = 0;
    for (size_t i = 0; i < pattern.size(); ++i) {
        index += hash_multipliers[i] * state[pattern[i]];
    }
    return index;
}

size_t PatternDatabase::hash_index_abstracted(
    const std::vector<int> &abs_state) const
{
    size_t index = 0;
    for(size_t i = 0; i < pattern.size(); ++i) {
        index += hash_multipliers[i] * abs_state[i];
    }
    return index;
}

int
PatternDatabase::get_value(const GlobalState& state) const
{
    return distances[hash_index(state)];
}

int PatternDatabase::get_value_abstracted(
    const std::vector<int> &abstracted_state) const
{
    return distances[hash_index_abstracted(abstracted_state)];
}

int PatternDatabase::get_value_for_index(size_t index) const {
    return distances[index];
}

double
PatternDatabase::compute_mean_finite_h() const
{
    double sum = 0;
    int size = 0;
    for (size_t i = 0; i < distances.size(); ++i) {
        if (distances[i] != numeric_limits<int>::max()) {
            sum += distances[i];
            ++size;
        }
    }
    if (size == 0) { // All states are dead ends.
        return numeric_limits<double>::infinity();
    } else {
        return sum / size;
    }
}

bool
PatternDatabase::is_operator_relevant(const GlobalOperator& op) const
{
    for (const auto& effect : op.get_effects()) {
        int var_id = effect.var;
        if (binary_search(pattern.begin(), pattern.end(), var_id)) {
            return true;
        }
    }
    return false;
}
} // namespace pdbs
