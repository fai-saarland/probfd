#ifndef PROBFD_CARTESIAN_SUBTASK_GENERATORS_H
#define PROBFD_CARTESIAN_SUBTASK_GENERATORS_H

#include <memory>
#include <vector>

// Forward Declarations
struct FactPair;

namespace landmarks {
class LandmarkGraph;
}

namespace plugins {
class Options;
}

namespace utils {
class RandomNumberGenerator;
class LogProxy;
} // namespace utils

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::cartesian_abstractions {

using Facts = std::vector<FactPair>;
using SharedTasks = std::vector<std::shared_ptr<ProbabilisticTask>>;

enum class FactOrder { ORIGINAL, RANDOM, HADD_UP, HADD_DOWN };

/*
  Create focused subtasks.
*/
class SubtaskGenerator {
public:
    virtual SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        utils::LogProxy& log) const = 0;
    virtual ~SubtaskGenerator() = default;
};

/*
  Return copies of the original task.
*/
class TaskDuplicator : public SubtaskGenerator {
    int num_copies_;

public:
    explicit TaskDuplicator(const plugins::Options& opts);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        utils::LogProxy& log) const override;
};

/*
  Use ModifiedGoalsTask to return a subtask for each goal fact.
*/
class GoalDecomposition : public SubtaskGenerator {
    FactOrder fact_order_;
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit GoalDecomposition(const plugins::Options& opts);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        utils::LogProxy& log) const override;
};

/*
  Nest ModifiedGoalsTask and DomainAbstractedTask to return subtasks
  focussing on a single landmark fact.
*/
class LandmarkDecomposition : public SubtaskGenerator {
    FactOrder fact_order_;
    bool combine_facts_;
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit LandmarkDecomposition(const plugins::Options& opts);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        utils::LogProxy& log) const override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_SUBTASK_GENERATORS_H
