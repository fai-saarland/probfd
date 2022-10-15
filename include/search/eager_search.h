#ifndef EAGER_SEARCH_H
#define EAGER_SEARCH_H

#include <vector>
#include <memory>

#include "evaluator.h"
#include "global_state.h"
#include "search_engine.h"
#include "search_progress.h"
#include "search_space.h"

#include "open_lists/open_list.h"

class GlobalOperator;
class Heuristic;
class Evaluator;

class EagerSearch : public SearchEngine {
    // Search Behavior parameters
    bool reopen_closed_nodes; // whether to reopen closed nodes upon finding lower g paths
    bool do_pathmax; // whether to use pathmax correction
    bool use_multi_path_dependence;

    std::shared_ptr<OpenList<StateID>> open_list;
    std::shared_ptr<Evaluator> f_evaluator;

protected:
    SearchStatus step() override;
    std::pair<SearchNode, bool> fetch_next_node();
    void update_jump_statistic(const SearchNode &node);
    void print_heuristic_values(const std::vector<int> &values) const;
    void reward_progress();

    std::vector<Heuristic *> heuristics;
    std::vector<std::shared_ptr<Heuristic> > preferred_operator_heuristics;
    std::vector<Heuristic *> estimate_heuristics;
    // TODO: in the long term this
    // should disappear into the open list

    virtual void initialize() override;
    virtual void statistics() const override;

public:
    EagerSearch(const options::Options &opts);

    void dump_search_space();
};

#endif
