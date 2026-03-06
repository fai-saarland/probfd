#include "probfd/cli/pdbs/fully_additive_finder_factory.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "probfd/pdbs/fully_additive_finder_factory.h"

using namespace downward;

using namespace probfd::pdbs;

using namespace language::parser;

namespace probfd::cli::pdbs {

InternalFunctionDefinitionBase&
add_fully_additive_finder_factory_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&construct_shared<
        SubCollectionFinderFactory,
        FullyAdditiveFinderFactory>>(nspace, "fully_additive_factory");

    return f;
}

} // namespace probfd::cli::pdbs
