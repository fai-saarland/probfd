#include "downward/search_algorithm.h"

#include "downward/evaluation_context.h"
#include "downward/evaluator.h"

#include "downward/algorithms/ordered_set.h"

#include "downward/task_utils/successor_generator.h"
#include "downward/task_utils/task_properties.h"

#include "downward/tasks/root_task.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <iostream>

using namespace std;
using downward::utils::ExitCode;

namespace downward {

class PruningMethod;

static successor_generator::SuccessorGenerator&
get_successor_generator(const TaskProxy& task_proxy, utils::LogProxy& log)
{
    log << "Building successor generator..." << flush;
    int peak_memory_before = utils::get_peak_memory_in_kb();
    utils::Timer successor_generator_timer;
    successor_generator::SuccessorGenerator& successor_generator =
        successor_generator::g_successor_generators[task_proxy];
    successor_generator_timer.stop();
    log << "done!" << endl;
    int peak_memory_after = utils::get_peak_memory_in_kb();
    int memory_diff = peak_memory_after - peak_memory_before;
    log << "peak memory difference for successor generator creation: "
        << memory_diff << " KB" << endl
        << "time for successor generation creation: "
        << successor_generator_timer << endl;
    return successor_generator;
}

SearchAlgorithm::SearchAlgorithm(
    OperatorCost cost_type,
    int bound,
    double max_time,
    const string& description,
    utils::Verbosity verbosity)
    : description(description)
    , solution_found(false)
    , task(tasks::g_root_task)
    , task_proxy(*task)
    , log(utils::get_log_for_verbosity(verbosity))
    , state_registry(task_proxy)
    , successor_generator(get_successor_generator(task_proxy, log))
    , search_space(state_registry, log)
    , statistics(log)
    , bound(bound)
    , cost_type(cost_type)
    , is_unit_cost(task_properties::is_unit_cost(task_proxy))
    , max_time(max_time)
{
    if (bound < 0) {
        cerr << "error: negative cost bound " << bound << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
    task_properties::print_variable_statistics(task_proxy);
}

SearchAlgorithm::~SearchAlgorithm()
{
}

bool SearchAlgorithm::found_solution() const
{
    return solution_found;
}

const Plan& SearchAlgorithm::get_plan() const
{
    assert(solution_found);
    return plan;
}

void SearchAlgorithm::set_plan(const Plan& p)
{
    solution_found = true;
    plan = p;
}

bool SearchAlgorithm::check_goal_and_set_plan(const State& state)
{
    if (task_properties::is_goal_state(task_proxy, state)) {
        log << "Solution found!" << endl;
        Plan plan;
        search_space.trace_path(state, plan);
        set_plan(plan);
        return true;
    }
    return false;
}

void SearchAlgorithm::save_plan_if_necessary()
{
    if (found_solution()) { plan_manager.save_plan(get_plan(), task_proxy); }
}

int SearchAlgorithm::get_adjusted_cost(const OperatorProxy& op) const
{
    return get_adjusted_action_cost(op, cost_type, is_unit_cost);
}

void print_initial_evaluator_values(const EvaluationContext& eval_context)
{
    eval_context.get_cache().for_each_evaluator_result(
        [](const Evaluator* eval, const EvaluationResult& result) {
            if (eval->is_used_for_reporting_minima()) {
                eval->report_value_for_initial_state(result);
            }
        });
}

void collect_preferred_operators(
    EvaluationContext& eval_context,
    Evaluator* preferred_operator_evaluator,
    ordered_set::OrderedSet<OperatorID>& preferred_operators)
{
    if (!eval_context.is_evaluator_value_infinite(
            preferred_operator_evaluator)) {
        for (OperatorID op_id : eval_context.get_preferred_operators(
                 preferred_operator_evaluator)) {
            preferred_operators.insert(op_id);
        }
    }
}

} // namespace downward