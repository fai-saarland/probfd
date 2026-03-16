#include "downward/cli/search_algorithms/search_algorithm_factory_category.h"

#include "language/plugins/registry.h"

#include "downward/search_algorithm.h"
#include "downward/task_dependent_factory_fwd.h"

using namespace language::plugins;

namespace downward::cli::search_algorithms {

void add_search_algorithm_factory_category(Namespace& nspace)
{
    nspace
        .insert_shared_type_declaration<TaskDependentFactory<SearchAlgorithm>>(
            "SearchAlgorithmFactory",
            "");
}

} // namespace downward::cli::search_algorithms