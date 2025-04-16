#ifndef CEGAR_SUBTASK_GENERATORS_H
#define CEGAR_SUBTASK_GENERATORS_H

#include "downward/task_dependent_factory_fwd.h"

#include "downward/cartesian_abstractions/types.h"

#include <downward/task_dependent_factory.h>
#include <memory>
#include <vector>

namespace downward {
class MutexInformation;
class AbstractTask;
struct FactPair;

class StateMapping;
class InverseOperatorMapping;
}

namespace downward::landmarks {
class LandmarkGraph;
class LandmarkNode;
}

namespace downward::utils {
class RandomNumberGenerator;
class LogProxy;
} // namespace utils

namespace downward::cartesian_abstractions {
using Facts = std::vector<FactPair>;

enum class FactOrder { ORIGINAL, RANDOM, HADD_UP, HADD_DOWN };

/*
  Create focused subtasks.
*/
class SubtaskGenerator {
public:
    virtual SharedTasks get_subtasks(
        const std::shared_ptr<AbstractTask>& task,
        utils::LogProxy& log) const = 0;
    virtual ~SubtaskGenerator() = default;
};

/*
  Return copies of the original task.
*/
class TaskDuplicator : public SubtaskGenerator {
    int num_copies;

public:
    explicit TaskDuplicator(int copies);

    virtual SharedTasks get_subtasks(
        const std::shared_ptr<AbstractTask>& task,
        utils::LogProxy& log) const override;
};

/*
  Use ModifiedGoalsTask to return a subtask for each goal fact.
*/
class GoalDecomposition : public SubtaskGenerator {
    FactOrder fact_order;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit GoalDecomposition(FactOrder order, int random_seed);

    virtual SharedTasks get_subtasks(
        const std::shared_ptr<AbstractTask>& task,
        utils::LogProxy& log) const override;
};

/*
  Nest ModifiedGoalsTask and DomainAbstractedTask to return subtasks
  focussing on a single landmark fact.
*/
class LandmarkDecomposition : public SubtaskGenerator {
    std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory;
    FactOrder fact_order;
    bool combine_facts;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    /* Perform domain abstraction by combining facts that have to be
       achieved before a given landmark can be made true. */
    std::shared_ptr<AbstractTask> build_domain_abstracted_task(
        const std::shared_ptr<AbstractTask>& parent,
        const landmarks::LandmarkNode *node) const;

public:
    explicit LandmarkDecomposition(
        std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory,
        FactOrder order,
        int random_seed,
        bool combine_facts);

    virtual SharedTasks get_subtasks(
        const std::shared_ptr<AbstractTask>& task,
        utils::LogProxy& log) const override;
};
} // namespace cartesian_abstractions

#endif
