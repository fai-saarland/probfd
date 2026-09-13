#include "probfd/merge_and_shrink/merge_scoring_function_factory_goal_relevance.h"

#include "probfd/merge_and_shrink/merge_scoring_function_goal_relevance.h"

using namespace downward;

namespace probfd::merge_and_shrink {

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionFactoryGoalRelevance::compute_scoring_function(
    const FactoredTransitionSystem&)
{
    return std::make_unique<MergeScoringFunctionGoalRelevance>();
}

std::string MergeScoringFunctionFactoryGoalRelevance::name() const
{
    return "goal relevance";
}

} // namespace probfd::merge_and_shrink