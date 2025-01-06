#ifndef SEARCH_ALGORITHMS_ITERATED_SEARCH_H
#define SEARCH_ALGORITHMS_ITERATED_SEARCH_H

#include "downward/search_algorithm.h"

#include <memory>
#include <vector>

class SearchAlgorithmFactory;

namespace iterated_search {

class IteratedSearch : public SearchAlgorithm {
    std::vector<std::shared_ptr<SearchAlgorithmFactory>> algorithm_configs;

    bool pass_bound;
    bool repeat_last_phase;
    bool continue_on_fail;
    bool continue_on_solve;

    int phase;
    bool last_phase_found_solution;
    int best_bound;
    bool iterated_found_solution;

    std::shared_ptr<SearchAlgorithm>
    get_search_algorithm(int algorithm_configs_index);
    std::shared_ptr<SearchAlgorithm> create_current_phase();
    SearchStatus step_return_value();

    virtual SearchStatus step() override;

public:
    IteratedSearch(
        OperatorCost operator_cost,
        int bound,
        double max_time,
        std::string description,
        utils::Verbosity verbosity,
        std::vector<std::shared_ptr<SearchAlgorithmFactory>> algorithm_configs,
        bool pass_bound,
        bool repeat_last,
        bool continue_on_fail,
        bool continue_on_solve);

    ~IteratedSearch() override;

    virtual void save_plan_if_necessary() override;
    virtual void print_statistics() const override;
};
} // namespace iterated_search

#endif
