#include "probfd/cli/pdbs/cegar/sampling_flaw_finder.h"

#include "language/ast/internal_function_definition.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/pdbs/cegar/sampling_flaw_finder.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs::cegar;

using namespace language::parser;

using downward::cli::utils::add_rng_options_to_feature;

namespace probfd::cli::pdbs::cegar {

InternalFunctionDefinitionBase&
add_sampling_flaw_finder_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        FlawFindingStrategy,
        SamplingFlawFinder,
        std::shared_ptr<RandomNumberGenerator>,
        int>>(nspace, "sampling_flaw_finder");

    const auto n = add_rng_options_to_feature(f, 0);
    f.make_optional_argument_with_default(
        n,
        "max_search_states",
        "20M",
        "Maximal number of generated states after which the flaw search is "
        "aborted.");

    return f;
}

} // namespace probfd::cli::pdbs::cegar
