#ifndef SEARCH_ALGORITHMS_EAGER_SEARCH_H
#define SEARCH_ALGORITHMS_EAGER_SEARCH_H

#include "downward/open_list.h"
#include "downward/search_algorithm.h"

#include <memory>
#include <vector>

namespace downward {
class Evaluator;
class PruningMethod;
}

namespace downward::eager_search {
class EagerSearch : public SearchAlgorithm {
    const bool reopen_closed_nodes;

    std::unique_ptr<StateOpenList> open_list;
    std::shared_ptr<Evaluator> f_evaluator;

    std::vector<Evaluator*> path_dependent_evaluators;
    std::vector<std::shared_ptr<Evaluator>> preferred_operator_evaluators;
    std::shared_ptr<Evaluator> lazy_evaluator;

    std::shared_ptr<PruningMethod> pruning_method;

    void start_f_value_statistics(EvaluationContext& eval_context);
    void update_f_value_statistics(EvaluationContext& eval_context);
    void reward_progress();

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit EagerSearch(
        std::unique_ptr<StateOpenList> open,
        bool reopen_closed,
        std::shared_ptr<Evaluator> f_eval,
        std::vector<std::shared_ptr<Evaluator>> preferred,
        std::shared_ptr<Evaluator> lazy_evaluator,
        std::shared_ptr<PruningMethod> pruning,
        OperatorCost cost_type,
        int bound,
        double max_time,
        const std::string& description,
        utils::Verbosity verbosity);

    virtual void print_statistics() const override;

    void dump_search_space() const;
};

} // namespace eager_search

#endif
