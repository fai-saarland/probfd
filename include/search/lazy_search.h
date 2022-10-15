#ifndef LAZY_SEARCH_H
#define LAZY_SEARCH_H

#include "global_state.h"
#include "evaluator.h"
#include "search_engine.h"
#include "search_progress.h"
#include "search_space.h"

#include "open_lists/open_list.h"

#include <vector>
#include <memory>

class GlobalOperator;
class Heuristic;

using OpenListEntryLazy = std::pair<StateID, const GlobalOperator *>;

class LazySearch : public SearchEngine {
protected:
    std::shared_ptr<OpenList<OpenListEntryLazy> > open_list;

    // Search Behavior parameters
    bool reopen_closed_nodes; // whether to reopen closed nodes upon finding lower g paths
    bool randomize_successors;
    bool preferred_successors_first;

    std::vector<Heuristic *> heuristics;
    std::vector<std::shared_ptr<Heuristic> > preferred_operator_heuristics;
    std::vector<Heuristic *> estimate_heuristics;

    GlobalState current_state;
    StateID current_predecessor_id;
    const GlobalOperator *current_operator;
    int current_g;
    int current_real_g;

    virtual void initialize() override;
    virtual SearchStatus step() override;

    void generate_successors();
    SearchStatus fetch_next_state();

    void reward_progress();

    void get_successor_operators(std::vector<const GlobalOperator *> &ops);

    virtual void statistics() const override;
public:

    LazySearch(const options::Options &opts);
    virtual ~LazySearch();
    void set_pref_operator_heuristics(std::vector<std::shared_ptr<Heuristic> > &heur);

};

#endif
