#include "probfd/cli/merge_and_shrink/merge_selector_score_based_filtering.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_selector_score_based_filtering.h"

#include "probfd/merge_and_shrink/merge_scoring_function.h"

using namespace std;
using namespace language::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_merge_selector_score_based_filtering_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pscore_based_filtering",
        &language::plugins::construct_shared<
            MergeSelector,
            MergeSelectorScoreBasedFiltering,
            std::vector<std::shared_ptr<MergeScoringFunction>>>);

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

} // namespace probfd::cli::merge_and_shrink
