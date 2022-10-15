#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "solver_interface.h"
#include <vector>

class Heuristic;
class StateRegistry;

namespace options {
class OptionParser;
class Options;
}

#include "global_operator.h"
#include "operator_cost.h"
#include "search_space.h"
#include "search_progress.h"

enum SearchStatus {IN_PROGRESS, TIMEOUT, FAILED, SOLVED};

class SearchEngine : public SolverInterface {
public:
    typedef std::vector<const GlobalOperator *> Plan;
private:
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

    virtual void initialize() {}
    virtual SearchStatus step() = 0;
    virtual void statistics() const;

    void set_plan(const Plan &plan);
    bool check_goal_and_set_plan(const GlobalState &state);
    int get_adjusted_cost(const GlobalOperator &op) const;
public:
    SearchEngine(const options::Options &opts);
    virtual ~SearchEngine();
    void set_state_registry(std::shared_ptr<StateRegistry> state_registry);
    virtual void print_statistics() const final override;
    virtual void save_result_if_necessary() const override;
    virtual bool found_solution() const override;
    virtual void solve() override;

    virtual void save_plan_if_necessary() const;
    SearchStatus get_status() const;
    const Plan &get_plan() const;
    SearchProgress get_search_progress() const {return search_progress; }
    void set_bound(int b) {bound = b; }
    int get_bound() {return bound; }
    static void add_options_to_parser(options::OptionParser &parser);
};

#endif
