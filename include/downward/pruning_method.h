#ifndef PRUNING_METHOD_H
#define PRUNING_METHOD_H

#include "downward/operator_id.h"

#include "downward/utils/logging.h"
#include "downward/utils/timer.h"

#include <memory>
#include <vector>

namespace downward {
class AbstractTask;
class State;

namespace limited_pruning {
class LimitedPruning;
}

class PruningMethod {
    utils::Timer timer;
    friend class limited_pruning::LimitedPruning;

    virtual void prune(const State& state, std::vector<OperatorID>& op_ids) = 0;

protected:
    mutable utils::LogProxy log;
    std::shared_ptr<AbstractTask> task;
    long num_successors_before_pruning;
    long num_successors_after_pruning;

public:
    explicit PruningMethod(utils::Verbosity verbosity);
    virtual ~PruningMethod() = default;
    virtual void initialize(const std::shared_ptr<AbstractTask>& task);
    void prune_operators(const State& state, std::vector<OperatorID>& op_ids);
    virtual void print_statistics() const;
};
}

#endif
