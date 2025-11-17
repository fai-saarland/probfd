#include "downward/cli/merge_and_shrink/merge_scoring_function_goal_relevance_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/merge_and_shrink/merge_scoring_function_goal_relevance.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

namespace {
class MergeScoringFunctionGoalRelevanceFeature
    : public SharedTypedFeature<MergeScoringFunction> {
public:
    MergeScoringFunctionGoalRelevanceFeature()
        : TypedFeature(
              "goal_relevance",
              &MergeScoringFunctionGoalRelevanceFeature::func)
    {
        document_title("Goal relevance scoring");
        document_synopsis(
            "This scoring function assigns a merge candidate a value of 0 iff "
            "at "
            "least one of the two transition systems of the merge candidate is "
            "goal relevant in the sense that there is an abstract non-goal "
            "state. "
            "All other candidates get a score of positive infinity.");
    }

    static shared_ptr<MergeScoringFunction> func(const Context&)
    {
        return make_shared<MergeScoringFunctionGoalRelevance>();
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_goal_relevance_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<MergeScoringFunctionGoalRelevanceFeature>();
}

} // namespace downward::cli::merge_and_shrink
