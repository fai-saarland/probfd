#include "probfd/cli/cartesian_abstractions/flaw_generator_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cartesian_abstractions/flaw_generator.h"

using namespace language::plugins;

using namespace probfd::cartesian_abstractions;

namespace probfd::cli::cartesian_abstractions {

void add_flaw_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<FlawGeneratorFactory>(
        "FlawGeneratorFactory",
        "Factory for flaw generation algorithms used in the "
        "cartesian abstraction "
        "refinement loop");
}

} // namespace probfd::cli::cartesian_abstractions
