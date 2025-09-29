#ifndef SEARCH_ALGORITHMS_LAZY_SEARCH_H
#define SEARCH_ALGORITHMS_LAZY_SEARCH_H

#include "downward/evaluation_context.h"
#include "downward/evaluator.h"
#include "downward/open_list.h"
#include "downward/operator_id.h"
#include "downward/search_algorithm.h"
#include "downward/search_progress.h"
#include "downward/search_space.h"

#include "downward/utils/rng.h"

#include <memory>
#include <vector>

namespace downward::lazy_search {
class LazySearch : public IterativeSearchAlgorithm<LazySearch> {
protected:
    std::unique_ptr<EdgeOpenList> open_list;

    // Search behavior parameters
    bool reopen_closed_nodes; // whether to reopen closed nodes upon finding
                              // lower g paths
    bool randomize_successors;
    bool preferred_successors_first;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    std::vector<Evaluator*> path_dependent_evaluators;
    std::vector<std::shared_ptr<Evaluator>> preferred_operator_evaluators;

    State current_state;
    StateID current_predecessor_id;
    OperatorID current_operator_id;
    int current_g;
    int current_real_g;
    EvaluationContext current_eval_context;

public:
    LazySearch(
        std::unique_ptr<EdgeOpenList> open,
        bool reopen_closed,
        std::vector<std::shared_ptr<Evaluator>> evaluators,
        bool randomize_successors,
        bool preferred_successors_first,
        int random_seed,
        SharedAbstractTask task,
        OperatorCost cost_type,
        int bound,
        utils::Duration max_time,
        const std::string& description,
        utils::Verbosity verbosity);

    void print_statistics() const override;

private:
    friend class IterativeSearchAlgorithm;

    SearchStatus initialize();
    SearchStatus step();

    void generate_successors();
    SearchStatus fetch_next_state();

    void reward_progress();

    std::vector<OperatorID> get_successor_operators(
        const ordered_set::OrderedSet<OperatorID>& preferred_operators) const;
};
} // namespace lazy_search

#endif
