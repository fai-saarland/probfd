#include "downward/cli/open_lists/open_list_factory_category.h"

#include "downward/open_list.h"

#include "downward/task_dependent_factory.h"

#include "language/ast/internal_type_declaration.h"

using namespace std;

namespace downward::cli::open_lists {

void add_open_list_factory_category(
    language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<
        TaskDependentFactory<OpenList<StateOpenListEntry>>>(
        nspace,
        "StateOpenList",
        "");
    insert_shared_type_declaration<
        TaskDependentFactory<OpenList<EdgeOpenListEntry>>>(
        nspace,
        "EdgeOpenList",
        "");
}

} // namespace downward::cli::open_lists
