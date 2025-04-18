#ifndef SEARCH_ALGORITHMS_ENFORCED_HILL_CLIMBING_SEARCH_H
#define SEARCH_ALGORITHMS_ENFORCED_HILL_CLIMBING_SEARCH_H

#include "downward/evaluation_context.h"
#include "downward/open_list.h"
#include "downward/search_algorithm.h"

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace downward::enforced_hill_climbing_search {
enum class PreferredUsage { PRUNE_BY_PREFERRED, RANK_PREFERRED_FIRST };

/*
  Enforced hill-climbing with deferred evaluation.

  TODO: We should test if this lazy implementation really has any benefits over
  an eager one. We hypothesize that both versions need to evaluate and store
  the same states anyways.
*/
class EnforcedHillClimbingSearch
    : public IterativeSearchAlgorithm<EnforcedHillClimbingSearch> {
    std::unique_ptr<EdgeOpenList> open_list;

    std::shared_ptr<Evaluator> evaluator;
    std::vector<std::shared_ptr<Evaluator>> preferred_operator_evaluators;
    std::set<Evaluator*> path_dependent_evaluators;
    bool use_preferred;
    PreferredUsage preferred_usage;

    EvaluationContext current_eval_context;
    int current_phase_start_g;

    // Statistics
    std::map<int, std::pair<int, int>> d_counts;
    int num_ehc_phases;
    int last_num_expanded;

public:
    EnforcedHillClimbingSearch(
        const std::shared_ptr<Evaluator>& h,
        PreferredUsage preferred_usage,
        const std::vector<std::shared_ptr<Evaluator>>& preferred,
        std::shared_ptr<AbstractTask> task,
        OperatorCost cost_type,
        int bound,
        double max_time,
        const std::string& description,
        utils::Verbosity verbosity);

    virtual void print_statistics() const override;

private:
    friend class IterativeSearchAlgorithm;

    void insert_successor_into_open_list(
        const EvaluationContext& eval_context,
        int parent_g,
        OperatorID op_id,
        bool preferred);
    void expand(EvaluationContext& eval_context);
    void reach_state(const State& parent, OperatorID op_id, const State& state);
    SearchStatus ehc();

    void initialize();
    SearchStatus step();
};
} // namespace downward::enforced_hill_climbing_search

#endif
