#include "probfd/cli/pdbs/subcollection_finder_factory_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/pdbs/subcollection_finder_factory.h"

using namespace probfd::pdbs;

using namespace language::parser;

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<SubCollectionFinderFactory>(
        nspace,
        "SubCollectionFinderFactory",
        "");
}

} // namespace probfd::cli::pdbs
