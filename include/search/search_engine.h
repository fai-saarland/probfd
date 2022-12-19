#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <vector>

class Heuristic;
class StateRegistry;

namespace options {
class OptionParser;
class Options;
}

#include "global_operator.h"
#include "operator_cost.h"
#include "plan_manager.h"
#include "search_progress.h"
#include "search_space.h"

enum SearchStatus {IN_PROGRESS, TIMEOUT, FAILED, SOLVED};

class SearchEngine {
    SearchStatus status;
    bool solution_found;
    Plan plan;
protected:
    std::shared_ptr<StateRegistry> state_registry;
    SearchSpace search_space;
    SearchProgress search_progress;
    int bound;
    OperatorCost cost_type;
    double max_time;
    PlanManager plan_manager;

    virtual void initialize() {}
    virtual SearchStatus step() = 0;

    void set_plan(const Plan &plan);
    bool check_goal_and_set_plan(const GlobalState &state);
    int get_adjusted_cost(const GlobalOperator &op) const;
public:
    SearchEngine(const options::Options &opts);
    virtual ~SearchEngine();
    void set_state_registry(std::shared_ptr<StateRegistry> state_registry);
    virtual void print_statistics() const = 0;
    virtual void save_plan_if_necessary();
    bool found_solution() const;
    void search();

    SearchStatus get_status() const;
    const Plan &get_plan() const;
    SearchProgress get_search_progress() const {return search_progress; }
    void set_bound(int b) {bound = b; }
    int get_bound() {return bound; }
    PlanManager& get_plan_manager() { return plan_manager; }

    static void add_options_to_parser(options::OptionParser &parser);
};

#endif
