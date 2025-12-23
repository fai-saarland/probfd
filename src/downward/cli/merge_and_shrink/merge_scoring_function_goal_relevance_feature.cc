#include "downward/cli/merge_and_shrink/merge_scoring_function_goal_relevance_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/merge_scoring_function_goal_relevance.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace language::plugins;

namespace downward::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_merge_scoring_function_goal_relevance_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "goal_relevance",
        &language::plugins::construct_shared<
            MergeScoringFunction,
            MergeScoringFunctionGoalRelevance>);
    f.document_title("Goal relevance scoring");
    f.document_synopsis(
        "This scoring function assigns a merge candidate a value of 0 iff "
        "at "
        "least one of the two transition systems of the merge candidate is "
        "goal relevant in the sense that there is an abstract non-goal "
        "state. "
        "All other candidates get a score of positive infinity.");

    return f;
}

} // namespace downward::cli::merge_and_shrink
