#include "downward/cli/merge_and_shrink/merge_selector_score_based_filtering_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/merge_and_shrink/merge_selector_score_based_filtering.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace {
class MergeSelectorScoreBasedFilteringFeature
    : public SharedTypedFeature<
          MergeSelector,
          const std::vector<std::shared_ptr<MergeScoringFunction>>&> {
public:
    MergeSelectorScoreBasedFilteringFeature()
        : TypedFeature(
              "score_based_filtering",
              &MergeSelectorScoreBasedFilteringFeature::func)
    {
        document_title("Score based filtering merge selector");
        document_synopsis(
            "This merge selector has a list of scoring "
            "functions, which are used "
            "iteratively to compute scores for merge candidates, "
            "keeping the best "
            "ones (with minimal scores) until only one is left.");

        make_required_argument(
            0,
            "scoring_functions",
            "The list of scoring functions used to compute scores for "
            "candidates.");
    }

    static shared_ptr<MergeSelector> func(
        const Context&,
        const std::vector<std::shared_ptr<MergeScoringFunction>>&
            scoring_functions)
    {
        return make_shared<MergeSelectorScoreBasedFiltering>(scoring_functions);
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_selector_score_based_filtering_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<MergeSelectorScoreBasedFilteringFeature>();
}

} // namespace downward::cli::merge_and_shrink
