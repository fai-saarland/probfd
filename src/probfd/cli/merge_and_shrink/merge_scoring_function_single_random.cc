#include "probfd/cli/merge_and_shrink/merge_scoring_function_single_random.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace language::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace {

InternalFunctionDefinitionBase&
add_merge_scoring_function_single_random_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "psingle_random",
        &language::plugins::construct_shared<
            MergeScoringFunction,
            MergeScoringFunctionSingleRandom,
            std::shared_ptr<utils::RandomNumberGenerator>>);

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
