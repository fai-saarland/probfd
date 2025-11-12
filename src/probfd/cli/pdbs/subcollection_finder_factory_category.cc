#include "probfd/cli/pdbs/subcollection_finder_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/subcollection_finder_factory.h"

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(Registry& raw_registry)
{
    raw_registry.insert_shared_category_plugin<SubCollectionFinderFactory>(
        "SubCollectionFinderFactory", "");
}

} // namespace probfd::cli::pdbs
