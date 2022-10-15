#ifndef ENFORCED_HILL_CLIMBING_SEARCH_H
#define ENFORCED_HILL_CLIMBING_SEARCH_H

#include "g_evaluator.h"
#include "global_operator.h"
#include "global_state.h"
#include "globals.h"
#include "search_engine.h"
#include "search_node_info.h"
#include "search_progress.h"
#include "search_space.h"

#include "open_lists/open_list.h"

#include <map>
#include <vector>
#include <memory>

using OpenListEntryEHC = std::pair<StateID, std::pair<int, const GlobalOperator * > >;

enum PreferredUsage {
    PRUNE_BY_PREFERRED, RANK_PREFERRED_FIRST,
    MAX_PREFERRED_USAGE
};

class EnforcedHillClimbingSearch : public SearchEngine {
protected:
    std::shared_ptr<OpenList<OpenListEntryEHC> > open_list;
    std::shared_ptr<GEvaluator> g_evaluator;

    std::shared_ptr<Heuristic> heuristic;
    bool preferred_contains_eval;
    std::vector<std::shared_ptr<Heuristic> > preferred_heuristics;
    bool use_preferred;
    PreferredUsage preferred_usage;

    GlobalState current_state;
    int current_h;
    int current_g;

    // statistics
    std::map<int, std::pair<int, int> > d_counts;
    int num_ehc_phases;
    int last_expanded;

    virtual void initialize();
    virtual SearchStatus step();
    SearchStatus ehc();
    void get_successors(const GlobalState &state, std::vector<const GlobalOperator *> &ops);
    void evaluate(const GlobalState &parent, const GlobalOperator *op, const GlobalState &state);
    virtual void statistics() const;
public:
    EnforcedHillClimbingSearch(const options::Options &opts);
    virtual ~EnforcedHillClimbingSearch();

};

#endif
