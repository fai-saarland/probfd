#ifndef OPEN_LISTS_PARETO_OPEN_LIST_H
#define OPEN_LISTS_PARETO_OPEN_LIST_H

#include "downward/open_list_factory.h"

namespace downward::pareto_open_list {
class ParetoOpenListFactory : public OpenListFactory {
    std::vector<std::shared_ptr<Evaluator>> evals;
    bool state_uniform_selection;
    int random_seed;
    bool pref_only;

public:
    ParetoOpenListFactory(
        const std::vector<std::shared_ptr<Evaluator>>& evals,
        bool state_uniform_selection,
        int random_seed,
        bool pref_only);

    virtual std::unique_ptr<StateOpenList> create_state_open_list() override;
    virtual std::unique_ptr<EdgeOpenList> create_edge_open_list() override;
};
} // namespace pareto_open_list

#endif
