#include "probfd/cli/occupation_measures/constraint_generator_factory_category.h"

#include "language/plugins/registry.h"

#include "probfd/occupation_measures/constraint_generator.h"

using namespace probfd::occupation_measures;

using namespace language::plugins;

namespace probfd::cli::occupation_measures {

void add_constraint_generator_factory_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<ConstraintGenerator>(
        "OMConstraintGeneratorFactory",
        "");
}

} // namespace probfd::cli::occupation_measures
