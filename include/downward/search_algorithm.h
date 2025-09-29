#ifndef SEARCH_ALGORITHM_H
#define SEARCH_ALGORITHM_H

#include "downward/operator_cost.h"
#include "downward/operator_id.h"
#include "downward/plan_manager.h"
#include "downward/search_progress.h"
#include "downward/search_space.h"
#include "downward/search_statistics.h"
#include "downward/state.h"
#include "downward/state_registry.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"

#include <iostream>
#include <vector>

namespace downward {
namespace ordered_set {
template <typename T>
class OrderedSet;
}

namespace successor_generator {
class SuccessorGenerator;
}

enum SearchStatus {
    IN_PROGRESS,
    TIMEOUT,
    FAILED,
    SOLVED,
    UNSOLVABLE,
    UNSOLVABLE_INCOMPLETE
};

class SearchAlgorithm {
    std::string description;
    bool solution_found;
    Plan plan;

protected:
    // Hold a reference to the task implementation and pass it to objects
    // that need it.
    const SharedAbstractTask task;

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
    utils::Duration max_time;

    void set_plan(const Plan& plan);
    bool check_goal_and_set_plan(const State& state);
    int get_adjusted_cost(
        const OperatorProxy& op,
        const OperatorIntCostFunction& cost_function) const;

public:
    SearchAlgorithm(
        SharedAbstractTask task,
        OperatorCost cost_type,
        int bound,
        utils::Duration max_time,
        const std::string& description,
        utils::Verbosity verbosity);

    virtual ~SearchAlgorithm();
    virtual void print_statistics() const = 0;
    virtual void save_plan_if_necessary();
    bool found_solution() const;
    const Plan& get_plan() const;

    virtual void search() = 0;

    const SearchStatistics& get_statistics() const { return statistics; }

    void set_bound(int b) { bound = b; }

    int get_bound() const { return bound; }

    PlanManager& get_plan_manager() { return plan_manager; }

    std::string get_description() { return description; }
};

template <typename Derived>
class IterativeSearchAlgorithm : public SearchAlgorithm {
    SearchStatus status = SearchStatus::IN_PROGRESS;

public:
    using SearchAlgorithm::SearchAlgorithm;

protected:
    void search() override
    {
        status = static_cast<Derived&>(*this).initialize();

        if (status != IN_PROGRESS) { return; }

        utils::CountdownTimer timer(max_time);
        while (status == IN_PROGRESS) {
            status = static_cast<Derived&>(*this).step();
            if (timer.is_expired()) {
                log << "Time limit reached. Abort search." << std::endl;
                status = TIMEOUT;
                break;
            }
        }
        // TODO: Revise when and which search times are logged.
        log << "Actual search time: " << timer.get_elapsed_time() << std::endl;
    }

private:
    SearchStatus initialize() { return IN_PROGRESS; }
};

/*
  Print evaluator values of all evaluators evaluated in the evaluation
  context.
*/
extern void
print_initial_evaluator_values(const EvaluationContext& eval_context);

extern void collect_preferred_operators(
    EvaluationContext& eval_context,
    Evaluator* preferred_operator_evaluator,
    ordered_set::OrderedSet<OperatorID>& preferred_operators);
} // namespace downward

#endif
