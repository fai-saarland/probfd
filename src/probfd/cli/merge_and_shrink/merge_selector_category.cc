#include "probfd/cli/merge_and_shrink/merge_selector_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/merge_selector.h"

using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_selector_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<MergeSelector>("PMergeSelector");
    category.document_synopsis(
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
