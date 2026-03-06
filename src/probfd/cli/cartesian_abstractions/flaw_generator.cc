#include "probfd/cli/cartesian_abstractions/flaw_generator_category.h"

#include "probfd/cartesian_abstractions/flaw_generator.h"

#include "language/ast/internal_type_declaration.h"

using namespace language::parser;

using namespace probfd::cartesian_abstractions;

namespace probfd::cli::cartesian_abstractions {

void add_flaw_generator_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<FlawGeneratorFactory>(
        nspace,
        "FlawGeneratorFactory",
        "Factory for flaw generation algorithms used in the "
        "cartesian abstraction "
        "refinement loop");
}

} // namespace probfd::cli::cartesian_abstractions
