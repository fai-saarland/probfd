#include "downward/cli/merge_and_shrink/merge_selector_score_based_filtering_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/merge_selector_score_based_filtering.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase& add_merge_selector_score_based_filtering_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "score_based_filtering",
        &language::plugins::construct_shared<
            MergeSelector,
            MergeSelectorScoreBasedFiltering,
            const std::vector<std::shared_ptr<MergeScoringFunction>>&>);
    f.document_title("Score based filtering merge selector");
    f.document_synopsis(
        "This merge selector has a list of scoring "
        "functions, which are used "
        "iteratively to compute scores for merge candidates, "
        "keeping the best "
        "ones (with minimal scores) until only one is left.");

    f.make_required_argument(
        0,
        "scoring_functions",
        "The list of scoring functions used to compute scores for "
        "candidates.");

    return f;
}

} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_selector_score_based_filtering_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_merge_selector_score_based_filtering_to_namespace(n);
}

} // namespace downward::cli::merge_and_shrink
