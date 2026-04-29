#include "probfd/merge_and_shrink/merge_scoring_function_goal_relevance_factory.h"

#include "probfd/merge_and_shrink/merge_scoring_function_goal_relevance.h"

namespace probfd::merge_and_shrink {

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionGoalRelevanceFactory::create(SharedProbabilisticTask)
{ return std::make_unique<MergeScoringFunctionGoalRelevance>(); }

std::string MergeScoringFunctionGoalRelevanceFactory::name() const
{ return "goal relevance"; }

} // namespace probfd::merge_and_shrink