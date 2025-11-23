#include "probfd/cli/pdbs/subcollection_finder_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/subcollection_finder_factory.h"

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs {

void add_subcollection_finder_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<SubCollectionFinderFactory>(
        "SubCollectionFinderFactory",
        "");
}

} // namespace probfd::cli::pdbs
