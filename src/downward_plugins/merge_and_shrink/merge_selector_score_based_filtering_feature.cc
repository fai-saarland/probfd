#include "downward_plugins/plugins/plugin.h"

#include "downward/merge_and_shrink/merge_selector_score_based_filtering.h"

using namespace std;
using namespace utils;
using namespace merge_and_shrink;

using namespace downward_plugins::plugins;

namespace {

class MergeSelectorScoreBasedFilteringFeature
    : public TypedFeature<MergeSelector, MergeSelectorScoreBasedFiltering> {
public:
    MergeSelectorScoreBasedFilteringFeature()
        : TypedFeature("score_based_filtering")
    {
        document_title("Score based filtering merge selector");
        document_synopsis("This merge selector has a list of scoring "
                          "functions, which are used "
                          "iteratively to compute scores for merge candidates, "
                          "keeping the best "
                          "ones (with minimal scores) until only one is left.");

        add_list_option<shared_ptr<MergeScoringFunction>>(
            "scoring_functions",
            "The list of scoring functions used to compute scores for "
            "candidates.");
    }

    virtual shared_ptr<MergeSelectorScoreBasedFiltering>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared<MergeSelectorScoreBasedFiltering>(
            opts.get_list<shared_ptr<MergeScoringFunction>>(
                "scoring_functions"));
    }
};

FeaturePlugin<MergeSelectorScoreBasedFilteringFeature> _plugin;

} // namespace
