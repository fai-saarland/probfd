#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H

#include "downward/utils/logging.h"

#include <string>

namespace probfd {
class ProbabilisticTaskProxy;
class ProbabilisticTask;
} // namespace probfd

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class MergeStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeStrategyFactory {
protected:
    mutable utils::LogProxy log;

public:
    explicit MergeStrategyFactory(utils::Verbosity verbosity);

    virtual ~MergeStrategyFactory() = default;

    void dump_options() const;

    virtual std::unique_ptr<MergeStrategy> compute_merge_strategy(
        std::shared_ptr<ProbabilisticTask>& task,
        const FactoredTransitionSystem& fts) = 0;

    virtual bool requires_liveness() const = 0;
    virtual bool requires_goal_distances() const = 0;

protected:
    virtual std::string name() const = 0;
    virtual void dump_strategy_specific_options() const = 0;
};

} // namespace probfd::merge_and_shrink

#endif
