#include "probfd/cli/cartesian_abstractions/adaptive_flaw_generator.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cartesian_abstractions/adaptive_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace language::plugins;

using namespace probfd::cartesian_abstractions;

namespace probfd::cli::cartesian_abstractions {

InternalFunctionDefinitionBase&
add_adaptive_flaw_generator_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "flaws_adaptive",
        &language::plugins::construct_shared<
            FlawGeneratorFactory,
            AdaptiveFlawGeneratorFactory,
            std::vector<std::shared_ptr<FlawGeneratorFactory>>>);

    f.make_optional_argument_with_default(
        0,
        "generators",
        "[flaws_astar(), flaws_ilao()]",
        "The linear hierarchy of flaw generators.");

    return f;
}

} // namespace probfd::cli::cartesian_abstractions