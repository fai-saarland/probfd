#ifndef PRUNING_LIMITED_PRUNING_H
#define PRUNING_LIMITED_PRUNING_H

#include "downward/pruning_method.h"

namespace downward::limited_pruning {
class LimitedPruning : public PruningMethod {
    std::shared_ptr<PruningMethod> pruning_method;
    const double min_required_pruning_ratio;
    const int num_expansions_before_checking_pruning_ratio;
    int num_pruning_calls;
    bool is_pruning_disabled;

    virtual void
    prune(const State& state, std::vector<OperatorID>& op_ids) override;

public:
    explicit LimitedPruning(
        const std::shared_ptr<PruningMethod>& pruning,
        double min_required_pruning_ratio,
        int expansions_before_checking_pruning_ratio,
        utils::Verbosity verbosity);
    virtual void initialize(const std::shared_ptr<AbstractTask>&) override;
};
} // namespace limited_pruning

#endif
