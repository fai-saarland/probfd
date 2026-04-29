#include "probfd/merge_and_shrink/merge_scoring_function_single_random_factory.h"

#include "probfd/merge_and_shrink/merge_scoring_function_single_random.h"

namespace probfd::merge_and_shrink {

MergeScoringFunctionSingleRandomFactory::
    MergeScoringFunctionSingleRandomFactory(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
    : rng(std::move(rng))
{
}

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionSingleRandomFactory::create(SharedProbabilisticTask)
{ return std::make_unique<MergeScoringFunctionSingleRandom>(rng); }

std::string MergeScoringFunctionSingleRandomFactory::name() const
{ return "single random"; }

} // namespace probfd::merge_and_shrink