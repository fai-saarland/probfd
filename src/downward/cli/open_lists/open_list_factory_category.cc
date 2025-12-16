#include "downward/cli/open_lists/open_list_factory_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/open_list.h"

#include "downward/task_dependent_factory.h"

using namespace std;

namespace downward::cli::open_lists {

void add_open_list_factory_category(language::plugins::Registry& registry)
{
    language::plugins::Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<
        TaskDependentFactory<OpenList<StateOpenListEntry>>>(
        "StateOpenList",
        "");
    n.insert_shared_type_declaration<
        TaskDependentFactory<OpenList<EdgeOpenListEntry>>>("EdgeOpenList", "");
}

} // namespace downward::cli::open_lists
