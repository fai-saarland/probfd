#include "probfd/cli/cartesian_abstractions/policy_based_flaw_generator.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {

InternalFunctionDefinitionBase& add_ilao_flaw_generator_astar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "flaws_ilao",
        &cli::plugins::construct_shared<
            FlawGeneratorFactory,
            ILAOFlawGeneratorFactory,
            probfd::value_t,
            int>);

    f.make_optional_argument_with_default(
        0,
        "convergence_epsilon",
        "10e-4",
        "The tolerance for convergence checks.");

    f.make_optional_argument_with_default(
        1,
        "max_search_states",
        "infinity()",
        "maximum number of concrete states allowed to be generated during "
        "flaw "
        "search before giving up");

    return f;
}

} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_policy_based_flaw_generator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_ilao_flaw_generator_astar_to_namespace(n);
}

} // namespace probfd::cli::cartesian_abstractions