#ifndef PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/types.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {

/// This namespace contains heuristic implementations.
namespace heuristics {

class TaskDependentHeuristic : public TaskEvaluator {
protected:
    std::shared_ptr<ProbabilisticTask> task;
    ProbabilisticTaskProxy task_proxy;

    mutable utils::LogProxy log;

public:
    explicit TaskDependentHeuristic(const plugins::Options& options);

    TaskDependentHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log);

    virtual ~TaskDependentHeuristic() override = default;

    static void add_options_to_feature(plugins::Feature& feature);
};

} // namespace heuristics
} // namespace probfd

#endif