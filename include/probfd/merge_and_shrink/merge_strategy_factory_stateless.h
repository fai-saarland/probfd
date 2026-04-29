#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

namespace probfd::merge_and_shrink {
class MergeSelectorFactory;
}

namespace probfd::merge_and_shrink {

class MergeStrategyFactoryStateless : public MergeStrategyFactory {
    std::shared_ptr<MergeSelectorFactory> merge_selector;

public:
    explicit MergeStrategyFactoryStateless(
        std::shared_ptr<MergeSelectorFactory> merge_selector);

    std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts,
        downward::utils::LogProxy& log) override;

protected:
    std::string name() const override;

    void dump_strategy_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
