#include "downward/cli/merge_and_shrink/merge_scoring_function_goal_relevance_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/merge_scoring_function_goal_relevance.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase&
add_merge_scoring_function_goal_relevance_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "goal_relevance",
        &downward::cli::plugins::construct_shared<
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

} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_goal_relevance_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_merge_scoring_function_goal_relevance_to_namespace(n);
}

} // namespace downward::cli::merge_and_shrink
