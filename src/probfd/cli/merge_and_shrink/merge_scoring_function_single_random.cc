#include "probfd/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/utils/logging.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/utils/rng_options.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace {

class MergeScoringFunctionSingleRandomFeature
    : public TypedFeature<
          MergeScoringFunction,
          MergeScoringFunctionSingleRandom> {
public:
    MergeScoringFunctionSingleRandomFeature()
        : TypedFeature("psingle_random")
    {
        document_title("Single random");
        document_synopsis(
            "This scoring function assigns exactly one merge "
            "candidate a score of "
            "0, chosen randomly, and infinity to all others.");

        downward::cli::utils::add_rng_options_to_feature(*this);
    }

protected:
    shared_ptr<MergeScoringFunctionSingleRandom>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MergeScoringFunctionSingleRandom>(
            downward::cli::utils::get_rng_arguments_from_options(options));
    }
};

FeaturePlugin<MergeScoringFunctionSingleRandomFeature> _plugin;

} // namespace
