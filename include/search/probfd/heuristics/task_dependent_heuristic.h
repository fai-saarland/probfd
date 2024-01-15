#ifndef PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "probfd/evaluator.h"
#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>

namespace plugins {
class Feature;
} // namespace plugins

namespace probfd {

/// This namespace contains heuristic implementations.
namespace heuristics {

class TaskDependentHeuristic : public FDREvaluator {
protected:
    std::shared_ptr<ProbabilisticTask> task;
    ProbabilisticTaskProxy task_proxy;

    mutable utils::LogProxy log;

public:
    TaskDependentHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log);

    static void add_options_to_feature(plugins::Feature& feature);
};

} // namespace heuristics
} // namespace probfd

#endif