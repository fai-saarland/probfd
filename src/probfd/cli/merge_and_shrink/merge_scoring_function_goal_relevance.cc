#include "probfd/cli/merge_and_shrink/merge_scoring_function_goal_relevance.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/merge_and_shrink/merge_scoring_function_goal_relevance.h"

#include "probfd/merge_and_shrink/transition_system.h"

using namespace std;
using namespace language::parser;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_merge_scoring_function_goal_relevance_feature(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        MergeScoringFunction,
        MergeScoringFunctionGoalRelevance>>(nspace, "pgoal_relevance");

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
