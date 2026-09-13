#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_FACTORY_SCORE_BASED_FILTERING_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_FACTORY_SCORE_BASED_FILTERING_H

#include "probfd/merge_and_shrink/merge_selector_factory.h"

#include <memory>
#include <vector>

namespace probfd::merge_and_shrink {
class MergeScoringFunctionFactory;
}

namespace probfd::merge_and_shrink {

class MergeSelectorFactoryScoreBasedFiltering : public MergeSelectorFactory {
    std::vector<std::shared_ptr<MergeScoringFunctionFactory>>
        merge_scoring_function_factories;

public:
    explicit MergeSelectorFactoryScoreBasedFiltering(
        std::vector<std::shared_ptr<MergeScoringFunctionFactory>>
            merge_scoring_function_factories);

    std::unique_ptr<MergeSelector>
    compute_selector(const FactoredTransitionSystem& fts) override;

    bool requires_liveness() const override
    {
        return false;
    }

    bool requires_goal_distances() const override
    {
        return false;
    }

protected:
    std::string name() const override;

    void dump_selector_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
