#include "probfd/task_utils/task_properties.h"

#include "probfd/tasks/all_outcomes_determinization.h"

#include "probfd/probabilistic_task.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include "downward/abstract_task.h"

#include <iostream>
#include <limits>
#include <memory>
#include <ranges>
#include <unordered_map>
#include <utility>

using namespace std;
namespace vws = std::views;
using utils::ExitCode;

namespace probfd::task_properties {

static bool is_one(value_t value)
{
    return value == 1_vt;
}

bool is_unit_cost(const ProbabilisticTaskProxy& task)
{
    return std::ranges::all_of(
        task.get_operators() |
            vws::transform(&ProbabilisticOperatorProxy::get_cost),
        is_one);
}

value_t get_adjusted_action_cost(
    const ProbabilisticOperatorProxy& op,
    OperatorCost cost_type,
    bool is_unit_cost)
{
    switch (cost_type) {
    case NORMAL: return op.get_cost();
    case ONE: return 1_vt;
    case PLUSONE:
        if (is_unit_cost)
            return 1_vt;
        else
            return op.get_cost() + 1_vt;
    default: ABORT("Unknown cost type");
    }
}

static int
get_first_conditional_effects_op_id(const ProbabilisticTaskProxy& task)
{
    for (ProbabilisticOperatorProxy op : task.get_operators()) {
        for (ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            for (ProbabilisticEffectProxy effect : outcome.get_effects()) {
                if (!effect.get_conditions().empty()) return op.get_id();
            }
        }
    }
    return -1;
}

bool has_conditional_effects(const ProbabilisticTaskProxy& task)
{
    return get_first_conditional_effects_op_id(task) != -1;
}

void verify_no_conditional_effects(const ProbabilisticTaskProxy& task)
{
    int op_id = get_first_conditional_effects_op_id(task);
    if (op_id != -1) {
        ProbabilisticOperatorProxy op = task.get_operators()[op_id];
        cerr << "This configuration does not support conditional effects "
             << "(operator " << op.get_name() << ")!" << endl
             << "Terminating." << endl;
        utils::exit_with(ExitCode::SEARCH_UNSUPPORTED);
    }
}

vector<value_t> get_operator_costs(const ProbabilisticTaskProxy& task_proxy)
{
    vector<value_t> costs;
    ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    costs.reserve(operators.size());
    for (ProbabilisticOperatorProxy op : operators)
        costs.push_back(op.get_cost());
    return costs;
}

int get_num_total_effects(const ProbabilisticTaskProxy& task_proxy)
{
    int num_effects = 0;
    for (ProbabilisticOperatorProxy op : task_proxy.get_operators())
        for (ProbabilisticOutcomeProxy outcome : op.get_outcomes())
            num_effects += outcome.get_effects().size();
    num_effects += task_proxy.get_axioms().size();
    return num_effects;
}

void dump_task(const ProbabilisticTaskProxy& task_proxy)
{
    ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    value_t min_action_cost = numeric_limits<int>::max();
    value_t max_action_cost = 0;
    for (ProbabilisticOperatorProxy op : operators) {
        min_action_cost = min(min_action_cost, op.get_cost());
        max_action_cost = max(max_action_cost, op.get_cost());
    }
    utils::g_log << "Min action cost: " << min_action_cost << endl;
    utils::g_log << "Max action cost: " << max_action_cost << endl;

    VariablesProxy variables = task_proxy.get_variables();
    utils::g_log << "Variables (" << variables.size() << "):" << endl;
    for (VariableProxy var : variables) {
        utils::g_log << "  " << var.get_name() << " (range "
                     << var.get_domain_size() << ")" << endl;
        for (int val = 0; val < var.get_domain_size(); ++val) {
            utils::g_log << "    " << val << ": "
                         << var.get_fact(val).get_name() << endl;
        }
    }
    State initial_state = task_proxy.get_initial_state();
    utils::g_log << "Initial state (PDDL):" << endl;
    ::task_properties::dump_pddl(initial_state);
    utils::g_log << "Initial state (FDR):" << endl;
    ::task_properties::dump_fdr(initial_state);
    ::task_properties::dump_goals(task_proxy.get_goals());
}

static std::
    unordered_map<const ProbabilisticTask*, std::unique_ptr<AbstractTask>>
        determinizations_cache;

const AbstractTask& get_determinization(const ProbabilisticTask* task)
{
    if (determinizations_cache.find(task) == determinizations_cache.end()) {
        determinizations_cache.insert(make_pair(
            task,
            std::make_unique<tasks::AODDeterminizationTask>(task)));
    }

    return *determinizations_cache[task];
}

} // namespace probfd::task_properties
