#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_FACTORY_H

#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"

namespace probfd::merge_and_shrink {
class ShrinkStrategy;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunctionMIASMFactory : public MergeScoringFunctionFactory {
    const bool use_caching;
    const std::shared_ptr<ShrinkStrategy> shrink_strategy;
    const int max_states;
    const int max_states_before_merge;
    const int shrink_threshold_before_merge;

public:
    MergeScoringFunctionMIASMFactory(
        bool use_caching,
        std::shared_ptr<ShrinkStrategy> shrink_strategy,
        int max_states,
        int max_states_before_merge,
        int shrink_threshold_before_merge);

    std::unique_ptr<MergeScoringFunction>
    create(SharedProbabilisticTask task) override;

    std::string name() const override;

    void dump_function_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_FACTORY_H
