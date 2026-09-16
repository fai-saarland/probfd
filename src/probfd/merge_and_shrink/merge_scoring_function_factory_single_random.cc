#include "probfd/merge_and_shrink/merge_scoring_function_factory_single_random.h"

#include "probfd/merge_and_shrink/merge_scoring_function_single_random.h"

#include "probfd/merge_and_shrink/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeScoringFunctionFactorySingleRandom::
    MergeScoringFunctionFactorySingleRandom(
        std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng(std::move(rng))
{
}

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionFactorySingleRandom::compute_scoring_function(
    const FactoredTransitionSystem&)
{
    return std::make_unique<MergeScoringFunctionSingleRandom>(rng);
}

string MergeScoringFunctionFactorySingleRandom::name() const
{
    return "single random";
}

void MergeScoringFunctionFactorySingleRandom::dump_function_specific_options(
    utils::LogProxy&) const
{
}

} // namespace probfd::merge_and_shrink