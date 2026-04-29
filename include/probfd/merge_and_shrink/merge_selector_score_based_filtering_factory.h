#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_FACTORY_H

#include "probfd/merge_and_shrink/merge_selector_factory.h"

#include <memory>
#include <vector>

namespace probfd::merge_and_shrink {
class MergeScoringFunctionFactory;
}

namespace probfd::merge_and_shrink {

class MergeSelectorScoreBasedFilteringFactory : public MergeSelectorFactory {
    std::vector<std::shared_ptr<MergeScoringFunctionFactory>>
        merge_scoring_functions;

public:
    explicit MergeSelectorScoreBasedFilteringFactory(
        std::vector<std::shared_ptr<MergeScoringFunctionFactory>>
            scoring_functions);

    std::unique_ptr<MergeSelector>
    create(SharedProbabilisticTask task) override;

protected:
    std::string name() const override;

    void dump_selector_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
