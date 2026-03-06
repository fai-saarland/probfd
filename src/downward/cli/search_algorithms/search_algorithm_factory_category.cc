#include "downward/cli/search_algorithms/search_algorithm_factory_category.h"

#include "downward/search_algorithm.h"
#include "downward/task_dependent_factory_fwd.h"

#include "language/ast/internal_type_declaration.h"

using namespace language::parser;

namespace downward::cli::search_algorithms {

void add_search_algorithm_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<TaskDependentFactory<SearchAlgorithm>>(
        nspace,
        "SearchAlgorithmFactory",
        "");
}

} // namespace downward::cli::search_algorithms