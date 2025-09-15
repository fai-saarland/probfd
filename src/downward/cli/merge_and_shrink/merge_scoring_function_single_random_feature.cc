#include "downward/cli/merge_and_shrink/merge_scoring_function_single_random_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class MergeScoringFunctionSingleRandomFeature
    : public TypedFeature<
          MergeScoringFunction,
          MergeScoringFunctionSingleRandom> {
public:
    MergeScoringFunctionSingleRandomFeature()
        : TypedFeature("single_random")
    {
        document_title("Single random");
        document_synopsis(
            "This scoring function assigns exactly one merge "
            "candidate a score of "
            "0, chosen randomly, and infinity to all others.");

        add_rng_options_to_feature(*this);
    }

    virtual shared_ptr<MergeScoringFunctionSingleRandom>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<MergeScoringFunctionSingleRandom>(
            get_rng_arguments_from_options(opts));
    }
};
}

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_single_random_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<MergeScoringFunctionSingleRandomFeature>();
}

} // namespace
