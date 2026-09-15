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
    MergeScoringFunctionFactorySingleRandom(int random_seed)
    : random_seed(random_seed)
{
}

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionFactorySingleRandom::compute_scoring_function(
    const FactoredTransitionSystem&)
{
    return std::make_unique<MergeScoringFunctionSingleRandom>(
        utils::get_rng(random_seed));
}

string MergeScoringFunctionFactorySingleRandom::name() const
{
    return "single random";
}

void MergeScoringFunctionFactorySingleRandom::dump_function_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log.println("Random seed: {}", random_seed);
    }
}

} // namespace probfd::merge_and_shrink