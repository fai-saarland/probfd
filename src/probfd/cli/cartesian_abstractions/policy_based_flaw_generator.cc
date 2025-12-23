#include "probfd/cli/cartesian_abstractions/policy_based_flaw_generator.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace language::plugins;

using namespace probfd::cartesian_abstractions;

namespace probfd::cli::cartesian_abstractions {

InternalFunctionDefinitionBase&
add_policy_based_flaw_generator_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "flaws_ilao",
        &construct_shared<
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

} // namespace probfd::cli::cartesian_abstractions