#include "probfd/cli/occupation_measures/constraint_generator_factory_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/occupation_measures/constraint_generator.h"

using namespace probfd::occupation_measures;

using namespace language::parser;

namespace probfd::cli::occupation_measures {

void add_constraint_generator_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<ConstraintGenerator>(
        nspace,
        "OMConstraintGeneratorFactory",
        "");
}

} // namespace probfd::cli::occupation_measures
