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
};

extern void
add_task_dependent_heuristic_options_to_feature(plugins::Feature& feature);

extern std::tuple<utils::Verbosity>
get_task_dependent_heuristic_arguments_from_options(
    const plugins::Options& opts);

} // namespace probfd::heuristics

#endif