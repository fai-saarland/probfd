#ifndef PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "probfd/state_evaluator.h"
#include "probfd/task_proxy.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {

class TaskDependentHeuristic : public GlobalStateEvaluator {
protected:
    std::shared_ptr<ProbabilisticTask> task;
    ProbabilisticTaskProxy task_proxy;

public:
    explicit TaskDependentHeuristic(std::shared_ptr<ProbabilisticTask> task);
    explicit TaskDependentHeuristic(const options::Options& options);

    virtual ~TaskDependentHeuristic() override = default;

    static void add_options_to_parser(options::OptionParser& parser);
};

} // namespace heuristics
} // namespace probfd

#endif