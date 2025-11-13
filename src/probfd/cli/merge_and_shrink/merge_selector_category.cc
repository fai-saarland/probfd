#include "probfd/cli/merge_and_shrink/merge_selector_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_selector.h"

using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_selector_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<MergeSelector>(
        "PMergeSelector",
        "This page describes the available merge selectors. They are used "
        "to "
        "compute the next merge purely based on the state of the given "
        "factored "
        "transition system. They are used in the merge strategy of type "
        "'stateless', but they can also easily be used in different "
        "'combined' "
        "merged strategies.");
}

} // namespace probfd::cli::merge_and_shrink
