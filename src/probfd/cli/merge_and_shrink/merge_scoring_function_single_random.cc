#include "probfd/cli/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace {

Feature&
add_merge_scoring_function_single_random_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "psingle_random",
        &downward::cli::plugins::make_shared<
            MergeScoringFunction,
            MergeScoringFunctionSingleRandom,
            int>);

    f.document_title("Single random");
    f.document_synopsis(
        "This scoring function assigns exactly one merge "
        "candidate a score of "
        "0, chosen randomly, and infinity to all others.");

    downward::cli::utils::add_rng_options_to_feature(f, 0);

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_scoring_function_single_random_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_merge_scoring_function_single_random_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink
