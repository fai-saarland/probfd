#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_FACTORY_H

#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"

namespace probfd::merge_and_shrink {

class MergeScoringFunctionDFPFactory : public MergeScoringFunctionFactory {
public:
    std::unique_ptr<MergeScoringFunction>
    create(SharedProbabilisticTask task) override;

    std::string name() const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_FACTORY_H
