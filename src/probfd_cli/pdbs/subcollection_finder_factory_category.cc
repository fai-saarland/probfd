#include "probfd_cli/pdbs/subcollection_finder_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/pdbs/subcollection_finder_factory.h"

using namespace probfd::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<SubCollectionFinderFactory>(
        "SubCollectionFinderFactory");
}

} // namespace probfd::cli::pdbs
