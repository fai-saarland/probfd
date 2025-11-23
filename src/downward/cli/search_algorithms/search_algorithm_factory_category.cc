#include "downward/cli/search_algorithms/search_algorithm_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/search_algorithm.h"
#include "downward/task_dependent_factory_fwd.h"

using namespace downward::cli::plugins;

namespace downward::cli::search_algorithms {

void add_search_algorithm_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<TaskDependentFactory<SearchAlgorithm>>(
        "SearchAlgorithmFactory",
        "");
}

} // namespace downward::cli::search_algorithms