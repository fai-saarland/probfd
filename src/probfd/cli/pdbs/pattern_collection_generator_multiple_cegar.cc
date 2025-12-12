#include "probfd/cli/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/cli/pdbs/cegar/cegar_options.h"
#include "probfd/cli/pdbs/pattern_collection_generator_multiple_options.h"

#include "probfd/pdbs/pattern_collection_generator_multiple_cegar.h"

using namespace downward;
using namespace utils;

using namespace probfd::cli::pdbs;

using namespace probfd::pdbs;
using namespace probfd::pdbs::cegar;

using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_multiple_cegar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "ppdbs_multiple_cegar",
        &construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorMultipleCegar,
            probfd::value_t,
            std::shared_ptr<FlawFindingStrategy>,
            bool,
            int,
            int,
            FSeconds,
            FSeconds,
            FSeconds,
            double,
            bool,
            bool,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);

    f.make_optional_argument_with_default(
        0,
        "convergence_epsilon",
        "10e-8",
        "The tolerance for convergence checks.");

    f.make_optional_argument_with_default(
        1,
        "flaw_strategy",
        "pucs_flaw_finder()",
        "strategy used to find flaws in a policy");
    const auto n = add_cegar_wildcard_option_to_feature(f, 2);
    add_multiple_options_to_feature(f, n + 2);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_multiple_cegar_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_multiple_cegar_to_namespace(n);
}

} // namespace probfd::cli::pdbs
