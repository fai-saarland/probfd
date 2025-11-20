#include "downward/cli/merge_and_shrink/merge_scoring_function_single_random_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
class MergeScoringFunctionSingleRandomFeature
    : public SharedTypedFeature<MergeScoringFunction, int> {
public:
    MergeScoringFunctionSingleRandomFeature()
        : TypedFeature(
              "single_random",
              &MergeScoringFunctionSingleRandomFeature::func)
    {
        document_title("Single random");
        document_synopsis(
            "This scoring function assigns exactly one merge "
            "candidate a score of "
            "0, chosen randomly, and infinity to all others.");

        add_rng_options_to_feature(*this, 0);
    }

    static shared_ptr<MergeScoringFunction>
    func(int random_seed)
    {
        return make_shared_from_arg_tuples<MergeScoringFunctionSingleRandom>(
            random_seed);
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_single_random_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<MergeScoringFunctionSingleRandomFeature>();
}

} // namespace downward::cli::merge_and_shrink
