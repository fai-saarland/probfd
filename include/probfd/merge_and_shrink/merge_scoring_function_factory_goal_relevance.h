#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_GOAL_RELEVANCE_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_GOAL_RELEVANCE_H

#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"

namespace probfd::merge_and_shrink {

class MergeScoringFunctionFactoryGoalRelevance
    : public MergeScoringFunctionFactory {
public:
    std::unique_ptr<MergeScoringFunction>
    compute_scoring_function(const FactoredTransitionSystem& fts) override;

private:
    std::string name() const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_GOAL_RELEVANCE_H
