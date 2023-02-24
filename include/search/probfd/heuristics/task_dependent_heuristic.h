#ifndef PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/task_proxy.h"

#include "utils/logging.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

/// This namespace contains heuristic implementations.
namespace heuristics {

class TaskDependentHeuristic : public TaskEvaluator {
protected:
    std::shared_ptr<ProbabilisticTask> task;
    ProbabilisticTaskProxy task_proxy;

    mutable utils::LogProxy log;

public:
    explicit TaskDependentHeuristic(const options::Options& options);

    TaskDependentHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log);

    virtual ~TaskDependentHeuristic() override = default;

    static void add_options_to_parser(options::OptionParser& parser);
};

} // namespace heuristics
} // namespace probfd

#endif