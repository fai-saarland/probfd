#include "probfd/cli/merge_and_shrink/merge_selector_score_based_filtering.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/merge_selector_score_based_filtering.h"

#include "probfd/merge_and_shrink/merge_scoring_function.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace {
class MergeSelectorScoreBasedFilteringFeature
    : public TypedFeature<MergeSelector, MergeSelectorScoreBasedFiltering> {
public:
    MergeSelectorScoreBasedFilteringFeature()
        : TypedFeature("pscore_based_filtering")
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

protected:
    shared_ptr<MergeSelectorScoreBasedFiltering>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MergeSelectorScoreBasedFiltering>(
            options.get_list<std::shared_ptr<MergeScoringFunction>>(
                "scoring_functions"));
    }
};
}

namespace probfd::cli::merge_and_shrink {

void add_merge_selector_score_based_filtering_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<MergeSelectorScoreBasedFilteringFeature>();
}

} // namespace
