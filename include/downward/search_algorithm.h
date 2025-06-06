#ifndef SEARCH_ALGORITHM_H
#define SEARCH_ALGORITHM_H

#include "downward/operator_cost.h"
#include "downward/operator_id.h"
#include "downward/plan_manager.h"
#include "downward/search_progress.h"
#include "downward/search_space.h"
#include "downward/search_statistics.h"
#include "downward/state_registry.h"
#include "downward/task_proxy.h"

#include "downward/utils/logging.h"

#include <vector>

namespace downward {
namespace ordered_set {
template <typename T>
class OrderedSet;
}

namespace successor_generator {
class SuccessorGenerator;
}

enum SearchStatus { IN_PROGRESS, TIMEOUT, FAILED, SOLVED };

class SearchAlgorithm {
    std::string description;
    SearchStatus status;
    bool solution_found;
    Plan plan;

protected:
    // Hold a reference to the task implementation and pass it to objects that
    // need it.
    const std::shared_ptr<AbstractTask> task;
    // Use task_proxy to access task information.
    TaskProxy task_proxy;

    mutable utils::LogProxy log;
    PlanManager plan_manager;
    StateRegistry state_registry;
    const successor_generator::SuccessorGenerator& successor_generator;
    SearchSpace search_space;
    SearchProgress search_progress;
    SearchStatistics statistics;
    int bound;
    OperatorCost cost_type;
    bool is_unit_cost;
    double max_time;

    virtual void initialize() {}
    virtual SearchStatus step() = 0;

    void set_plan(const Plan& plan);
    bool check_goal_and_set_plan(const State& state);
    int get_adjusted_cost(const OperatorProxy& op) const;

public:
    SearchAlgorithm(
        OperatorCost cost_type,
        int bound,
        double max_time,
        const std::string& description,
        utils::Verbosity verbosity);

    virtual ~SearchAlgorithm();
    virtual void print_statistics() const = 0;
    virtual void save_plan_if_necessary();
    bool found_solution() const;
    SearchStatus get_status() const;
    const Plan& get_plan() const;
    void search();
    const SearchStatistics& get_statistics() const { return statistics; }
    void set_bound(int b) { bound = b; }
    int get_bound() { return bound; }
    PlanManager& get_plan_manager() { return plan_manager; }
    std::string get_description() { return description; }
};

/*
  Print evaluator values of all evaluators evaluated in the evaluation context.
*/
extern void
print_initial_evaluator_values(const EvaluationContext& eval_context);

extern void collect_preferred_operators(
    EvaluationContext& eval_context,
    Evaluator* preferred_operator_evaluator,
    ordered_set::OrderedSet<OperatorID>& preferred_operators);
}

#endif
