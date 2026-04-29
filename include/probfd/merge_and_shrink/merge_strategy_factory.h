#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H

#include "downward/utils/logging.h"
#include "probfd/probabilistic_task.h"

#include <string>

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class MergeStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeStrategyFactory {
public:
    virtual ~MergeStrategyFactory() = default;

    virtual std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts,
        downward::utils::LogProxy& log) = 0;

    void dump_options(downward::utils::LogProxy& log) const;

protected:
    virtual std::string name() const = 0;

    virtual void
    dump_strategy_specific_options(downward::utils::LogProxy& log) const = 0;
};

} // namespace probfd::merge_and_shrink

#endif
