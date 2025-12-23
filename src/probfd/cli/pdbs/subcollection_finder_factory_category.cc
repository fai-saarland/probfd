#include "probfd/cli/pdbs/subcollection_finder_factory_category.h"

#include "language/plugins/registry.h"

#include "probfd/pdbs/subcollection_finder_factory.h"

using namespace probfd::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<SubCollectionFinderFactory>(
        "SubCollectionFinderFactory",
        "");
}

} // namespace probfd::cli::pdbs
