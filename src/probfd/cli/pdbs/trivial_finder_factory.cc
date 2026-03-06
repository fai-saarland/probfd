#include "probfd/cli/pdbs/trivial_finder_factory.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/pdbs/trivial_finder_factory.h"

using namespace downward;

using namespace probfd::pdbs;

using namespace language::parser;

namespace probfd::cli::pdbs {

InternalFunctionDefinitionBase&
add_trivial_finder_factory_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<
        &construct_shared<SubCollectionFinderFactory, TrivialFinderFactory>>(
        nspace,
        "finder_trivial_factory");

    return f;
}

} // namespace probfd::cli::pdbs
