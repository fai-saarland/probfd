#include "downward/cli/merge_and_shrink/merge_scoring_function_single_random_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace downward::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_merge_scoring_function_single_random_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "single_random",
        &language::plugins::construct_shared<
            MergeScoringFunction,
            MergeScoringFunctionSingleRandom,
            std::shared_ptr<RandomNumberGenerator>>);
    f.document_title("Single random");
    f.document_synopsis(
        "This scoring function assigns exactly one merge "
        "candidate a score of "
        "0, chosen randomly, and infinity to all others.");

    add_rng_options_to_feature(f, 0);

    return f;
}

} // namespace downward::cli::merge_and_shrink
