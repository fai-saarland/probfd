#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_SINGLE_RANDOM_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_SINGLE_RANDOM_H

#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"

#include <memory>

namespace probfd::merge_and_shrink {

class MergeScoringFunctionFactorySingleRandom
    : public MergeScoringFunctionFactory {
    int random_seed;

public:
    explicit MergeScoringFunctionFactorySingleRandom(int random_seed);

    std::unique_ptr<MergeScoringFunction>
    compute_scoring_function(const FactoredTransitionSystem& fts) override;

private:
    std::string name() const override;

    void dump_function_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
