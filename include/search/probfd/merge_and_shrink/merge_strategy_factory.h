#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H

#include "downward/utils/logging.h"

#include <memory>
#include <string>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class MergeStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeStrategyFactory {
protected:
    mutable utils::LogProxy log;

    virtual std::string name() const = 0;
    virtual void dump_strategy_specific_options() const = 0;

public:
    explicit MergeStrategyFactory(const plugins::Options& options);
    virtual ~MergeStrategyFactory() = default;
    void dump_options() const;
    virtual std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const ProbabilisticTaskProxy& task_proxy,
        const FactoredTransitionSystem& fts) = 0;
    virtual bool requires_init_distances() const = 0;
    virtual bool requires_goal_distances() const = 0;
};

extern void add_merge_strategy_options_to_feature(plugins::Feature& feature);

} // namespace probfd::merge_and_shrink

#endif
