#include "probfd/merge_and_shrink/merge_selector_factory_score_based_filtering.h"

#include "probfd/merge_and_shrink/merge_selector_score_based_filtering.h"

#include "probfd/merge_and_shrink/merge_scoring_function.h"
#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeSelectorFactoryScoreBasedFiltering::
    MergeSelectorFactoryScoreBasedFiltering(
        std::vector<std::shared_ptr<MergeScoringFunctionFactory>>
            merge_scoring_function_factories)
    : merge_scoring_function_factories(
          std::move(merge_scoring_function_factories))
{
}

std::unique_ptr<MergeSelector>
MergeSelectorFactoryScoreBasedFiltering::compute_selector(
    const ProbabilisticTaskTuple& task)
{
    return std::make_unique<MergeSelectorScoreBasedFiltering>(
        merge_scoring_function_factories |
        std::views::transform(
            [&](const auto& factory) -> std::shared_ptr<MergeScoringFunction> {
                return factory->compute_scoring_function(task);
            }) |
        std::ranges::to<std::vector>());
}

string MergeSelectorFactoryScoreBasedFiltering::name() const
{
    return "score based filtering";
}

void MergeSelectorFactoryScoreBasedFiltering::dump_selector_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        for (const auto& scoring_function_factory :
             merge_scoring_function_factories) {
            scoring_function_factory->dump_options(log);
        }
    }
}

} // namespace probfd::merge_and_shrink