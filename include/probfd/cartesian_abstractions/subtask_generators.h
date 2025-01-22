#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H

#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
struct FactPair;
class MutexFactory;
}

namespace downward::landmarks {
class LandmarkGraph;
}

namespace downward::utils {
class RandomNumberGenerator;
class LogProxy;
} // namespace utils

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::cartesian_abstractions {

using Facts = std::vector<downward::FactPair>;
using SharedTasks = std::vector<std::shared_ptr<ProbabilisticTask>>;

enum class FactOrder { ORIGINAL, RANDOM, HADD_UP, HADD_DOWN };

/*
  Create focused subtasks.
*/
class SubtaskGenerator {
public:
    virtual SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        downward::utils::LogProxy& log) const = 0;
    virtual ~SubtaskGenerator() = default;
};

/*
  Return copies of the original task.
*/
class TaskDuplicator : public SubtaskGenerator {
    int num_copies_;

public:
    explicit TaskDuplicator(int copies);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        downward::utils::LogProxy& log) const override;
};

/*
  Use ModifiedGoalsTask to return a subtask for each goal fact.
*/
class GoalDecomposition : public SubtaskGenerator {
    FactOrder fact_order_;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit GoalDecomposition(FactOrder order, int random_seed);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        downward::utils::LogProxy& log) const override;
};

/*
  Nest ModifiedGoalsTask and DomainAbstractedTask to return subtasks
  focussing on a single landmark fact.
*/
class LandmarkDecomposition : public SubtaskGenerator {
    std::shared_ptr<downward::MutexFactory> mutex_factory;
    FactOrder fact_order_;
    bool combine_facts_;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit LandmarkDecomposition(
        std::shared_ptr<downward::MutexFactory> mutex_factory,
        FactOrder order,
        bool combine_facts,
        int random_seed);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H
