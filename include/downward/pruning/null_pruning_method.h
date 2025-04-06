#ifndef PRUNING_NULL_PRUNING_METHOD_H
#define PRUNING_NULL_PRUNING_METHOD_H

#include "downward/pruning_method.h"

namespace downward::null_pruning_method {
class NullPruningMethod : public PruningMethod {
    virtual void prune(const State&, std::vector<OperatorID>&) override {}

public:
    explicit NullPruningMethod(utils::Verbosity verbosity);
    virtual void initialize(const std::shared_ptr<AbstractTask>&) override;
    virtual void print_statistics() const override {}
};
} // namespace null_pruning_method

#endif
