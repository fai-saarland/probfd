#include "probfd/cli/merge_and_shrink/merge_scoring_function_goal_relevance.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_scoring_function_goal_relevance_factory.h"

using namespace std;
using namespace language::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_merge_scoring_function_goal_relevance_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pgoal_relevance",
        &language::plugins::construct_shared<
            MergeScoringFunctionFactory,
            MergeScoringFunctionGoalRelevanceFactory>);

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

} // namespace probfd::cli::merge_and_shrink
