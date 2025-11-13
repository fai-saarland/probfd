#include "downward/cli/open_lists/open_list_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/open_list.h"

#include "downward/task_dependent_factory.h"

using namespace std;

namespace downward::cli::open_lists {

void add_open_list_factory_category(plugins::Registry& registry)
{
    plugins::Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<
        TaskDependentFactory<OpenList<StateOpenListEntry>>>(
        "StateOpenList",
        "");
    n.insert_shared_category_plugin<
        TaskDependentFactory<OpenList<EdgeOpenListEntry>>>("EdgeOpenList", "");
}

} // namespace downward::cli::open_lists
