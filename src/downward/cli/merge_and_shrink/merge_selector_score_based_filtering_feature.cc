#include "downward/cli/merge_and_shrink/merge_selector_score_based_filtering_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/merge_and_shrink/merge_selector_score_based_filtering.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace {
class MergeSelectorScoreBasedFilteringFeature
    : public TypedFeature<MergeSelector> {
public:
    MergeSelectorScoreBasedFilteringFeature()
        : TypedFeature("score_based_filtering")
    {
        document_title("Score based filtering merge selector");
        document_synopsis(
            "This merge selector has a list of scoring "
            "functions, which are used "
            "iteratively to compute scores for merge candidates, "
            "keeping the best "
            "ones (with minimal scores) until only one is left.");

        add_list_option<shared_ptr<MergeScoringFunction>>(
            "scoring_functions",
            "The list of scoring functions used to compute scores for "
            "candidates.");
    }

    virtual shared_ptr<MergeSelector>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared<MergeSelectorScoreBasedFiltering>(
            opts.get_list<shared_ptr<MergeScoringFunction>>(
                "scoring_functions"));
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_selector_score_based_filtering_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<MergeSelectorScoreBasedFilteringFeature>();
}

} // namespace downward::cli::merge_and_shrink
