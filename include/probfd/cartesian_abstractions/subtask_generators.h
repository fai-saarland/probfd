#ifndef PROBFD_CARTESIAN_SUBTASK_GENERATORS_H
#define PROBFD_CARTESIAN_SUBTASK_GENERATORS_H

#include "probfd/cartesian_abstractions/types.h"

#include <memory>
#include <vector>

// Forward Declarations
struct FactPair;
class MutexFactory;

namespace landmarks {
class LandmarkGraph;
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
    explicit TaskDuplicator(int copies);

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
    explicit GoalDecomposition(FactOrder order, int random_seed);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        utils::LogProxy& log) const override;
};

/*
  Nest ModifiedGoalsTask and DomainAbstractedTask to return subtasks
  focussing on a single landmark fact.
*/
class LandmarkDecomposition : public SubtaskGenerator {
    std::shared_ptr<MutexFactory> mutex_factory;
    FactOrder fact_order_;
    bool combine_facts_;
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit LandmarkDecomposition(
        std::shared_ptr<MutexFactory> mutex_factory,
        FactOrder order,
        bool combine_facts,
        int random_seed);

    SharedTasks get_subtasks(
        const std::shared_ptr<ProbabilisticTask>& task,
        utils::LogProxy& log) const override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_SUBTASK_GENERATORS_H
