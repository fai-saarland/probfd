#ifndef PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "probfd/evaluator.h"
#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>

// Forward Declarations
namespace plugins {
class Feature;
} // namespace plugins

/// This namespace contains heuristic implementations.
namespace probfd::heuristics {

class TaskDependentHeuristic : public FDREvaluator {
protected:
    std::shared_ptr<ProbabilisticTask> task_;
    ProbabilisticTaskProxy task_proxy_;

    mutable utils::LogProxy log_;

public:
    TaskDependentHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log);

    static void add_options_to_feature(plugins::Feature& feature);
};

} // namespace probfd::heuristics

#endif