#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_FACTORY_H

#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunctionSingleRandomFactory
    : public MergeScoringFunctionFactory {
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng;

public:
    explicit MergeScoringFunctionSingleRandomFactory(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    std::unique_ptr<MergeScoringFunction>
    create(SharedProbabilisticTask task) override;

private:
    std::string name() const override;
};

} // namespace probfd::merge_and_shrink

#endif
