#include "probfd/cli/pdbs/cegar/sampling_flaw_finder.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/pdbs/cegar/sampling_flaw_finder.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_sampling_flaw_finder_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "sampling_flaw_finder",
        &cli::plugins::construct_shared<
            SamplingFlawFinder,
            SamplingFlawFinder,
            std::shared_ptr<RandomNumberGenerator>,
            int>);

    const auto n = add_rng_options_to_feature(f, 0);
    f.make_optional_argument_with_default(
        n,
        "max_search_states",
        "20M",
        "Maximal number of generated states after which the flaw search is "
        "aborted.");

    return f;
}

} // namespace

namespace probfd::cli::pdbs::cegar {

void add_sampling_flaw_finder_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_sampling_flaw_finder_to_namespace(n);
}

} // namespace probfd::cli::pdbs::cegar
