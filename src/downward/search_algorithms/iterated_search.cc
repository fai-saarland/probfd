#include "downward/search_algorithms/iterated_search.h"

#include "downward/task_dependent_factory.h"

#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace downward::iterated_search {

IteratedSearch::IteratedSearch(
    OperatorCost operator_cost,
    int bound,
    double max_time,
    std::string description,
    utils::Verbosity verbosity,
    std::vector<std::shared_ptr<TaskDependentFactory<SearchAlgorithm>>>
        algorithm_configs,
    bool pass_bound,
    bool repeat_last,
    bool continue_on_fail,
    bool continue_on_solve)
    : IterativeSearchAlgorithm(
          operator_cost,
          bound,
          max_time,
          std::move(description),
          verbosity)
    , algorithm_configs(std::move(algorithm_configs))
    , pass_bound(pass_bound)
    , repeat_last_phase(repeat_last)
    , continue_on_fail(continue_on_fail)
    , continue_on_solve(continue_on_solve)
    , phase(0)
    , last_phase_found_solution(false)
    , best_bound(bound)
    , iterated_found_solution(false)
{
}

IteratedSearch::~IteratedSearch() = default;

shared_ptr<SearchAlgorithm>
IteratedSearch::get_search_algorithm(int algorithm_configs_index)
{
    auto& algorithm_factory = algorithm_configs[algorithm_configs_index];
    shared_ptr algorithm = algorithm_factory->create_object(task);
    log << "Starting search: " << algorithm->get_description() << endl;
    return algorithm;
}

shared_ptr<SearchAlgorithm> IteratedSearch::create_current_phase()
{
    int num_phases = algorithm_configs.size();
    if (phase >= num_phases) {
        /* We've gone through all searches. We continue if
           repeat_last_phase is true, but *not* if we didn't find a
           solution the last time around, since then this search would
           just behave the same way again (assuming determinism, which
           we might not actually have right now, but strive for). So
           this overrides continue_on_fail.
        */
        if (repeat_last_phase && last_phase_found_solution) {
            return get_search_algorithm(algorithm_configs.size() - 1);
        } else {
            return nullptr;
        }
    }

    return get_search_algorithm(phase);
}

SearchStatus IteratedSearch::step()
{
    shared_ptr<SearchAlgorithm> current_search = create_current_phase();
    if (!current_search) { return found_solution() ? SOLVED : FAILED; }
    if (pass_bound && best_bound < current_search->get_bound()) {
        current_search->set_bound(best_bound);
    }
    ++phase;

    current_search->search();

    Plan found_plan;
    int plan_cost = 0;
    last_phase_found_solution = current_search->found_solution();
    if (last_phase_found_solution) {
        iterated_found_solution = true;
        found_plan = current_search->get_plan();
        plan_cost = calculate_plan_cost(found_plan, task_proxy);
        if (plan_cost < best_bound) {
            plan_manager.save_plan(found_plan, task_proxy, true);
            best_bound = plan_cost;
            set_plan(found_plan);
        }
    }
    current_search->print_statistics();

    const SearchStatistics& current_stats = current_search->get_statistics();
    statistics.inc_expanded(current_stats.get_expanded());
    statistics.inc_evaluated_states(current_stats.get_evaluated_states());
    statistics.inc_evaluations(current_stats.get_evaluations());
    statistics.inc_generated(current_stats.get_generated());
    statistics.inc_generated_ops(current_stats.get_generated_ops());
    statistics.inc_reopened(current_stats.get_reopened());

    return step_return_value();
}

SearchStatus IteratedSearch::step_return_value()
{
    if (iterated_found_solution)
        log << "Best solution cost so far: " << best_bound << endl;

    if (last_phase_found_solution) {
        if (continue_on_solve) {
            log << "Solution found - keep searching" << endl;
            return IN_PROGRESS;
        } else {
            log << "Solution found - stop searching" << endl;
            return SOLVED;
        }
    } else {
        if (continue_on_fail) {
            log << "No solution found - keep searching" << endl;
            return IN_PROGRESS;
        } else {
            log << "No solution found - stop searching" << endl;
            return iterated_found_solution ? SOLVED : FAILED;
        }
    }
}

void IteratedSearch::print_statistics() const
{
    log << "Cumulative statistics:" << endl;
    statistics.print_detailed_statistics();
}

void IteratedSearch::save_plan_if_necessary()
{
    // We don't need to save here, as we automatically save after
    // each successful search iteration.
}

} // namespace downward::iterated_search
