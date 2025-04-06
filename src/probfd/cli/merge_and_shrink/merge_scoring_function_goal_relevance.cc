#include "probfd/merge_and_shrink/merge_scoring_function_goal_relevance.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace {

class MergeScoringFunctionGoalRelevanceFeature
    : public TypedFeature<
          MergeScoringFunction,
          MergeScoringFunctionGoalRelevance> {
public:
    MergeScoringFunctionGoalRelevanceFeature()
        : TypedFeature("pgoal_relevance")
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

    shared_ptr<MergeScoringFunctionGoalRelevance>
    create_component(const Options&, const utils::Context&) const override
    {
        return make_shared<MergeScoringFunctionGoalRelevance>();
    }
};

FeaturePlugin<MergeScoringFunctionGoalRelevanceFeature> _plugin;

} // namespace
