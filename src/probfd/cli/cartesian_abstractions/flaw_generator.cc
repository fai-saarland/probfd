#include "probfd/cli/cartesian_abstractions/flaw_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cartesian_abstractions/flaw_generator.h"

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace probfd::cli::cartesian_abstractions {

void add_flaw_generator_category(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<FlawGeneratorFactory>(
        "FlawGeneratorFactory");
    category.document_synopsis(
        "Factory for flaw generation algorithms used in the "
        "cartesian abstraction "
        "refinement loop");
}

} // namespace probfd::cli::cartesian_abstractions
