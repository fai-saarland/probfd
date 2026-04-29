#include "probfd/merge_and_shrink/merge_selector_score_based_filtering_factory.h"

#include "probfd/merge_and_shrink/merge_scoring_function.h"
#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"
#include "probfd/merge_and_shrink/merge_selector_score_based_filtering.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeSelectorScoreBasedFilteringFactory::
    MergeSelectorScoreBasedFilteringFactory(
        std::vector<shared_ptr<MergeScoringFunctionFactory>> scoring_functions)
    : merge_scoring_functions(std::move(scoring_functions))
{
}

std::unique_ptr<MergeSelector>
MergeSelectorScoreBasedFilteringFactory::create(SharedProbabilisticTask task)
{
    auto sfs = merge_scoring_functions |
               std::views::transform(
                   [task = std::move(task)](const auto& factory_ptr)
                       -> std::shared_ptr<MergeScoringFunction> {
                       return factory_ptr->create(task);
                   }) |
               std::ranges::to<std::vector>();

    return std::make_unique<MergeSelectorScoreBasedFiltering>(std::move(sfs));
}

string MergeSelectorScoreBasedFilteringFactory::name() const
{ return "score based filtering"; }

void MergeSelectorScoreBasedFilteringFactory::dump_selector_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        for (const auto& scoring_function : merge_scoring_functions) {
            scoring_function->dump_options(log);
        }
    }
}

} // namespace probfd::merge_and_shrink