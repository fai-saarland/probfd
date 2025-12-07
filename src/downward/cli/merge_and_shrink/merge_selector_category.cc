#include "downward/cli/merge_and_shrink/merge_selector_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/merge_selector.h"

using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace downward::cli::merge_and_shrink {

void add_merge_selector_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<MergeSelector>(
        "MergeSelector",
        "This page describes the available merge selectors. They are used "
        "to "
        "compute the next merge purely based on the state of the given "
        "factored "
        "transition system. They are used in the merge strategy of type "
        "'stateless', but they can also easily be used in different "
        "'combined' "
        "merged strategies.");
}

} // namespace downward::cli::merge_and_shrink
