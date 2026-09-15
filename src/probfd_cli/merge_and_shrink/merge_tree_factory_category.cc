#include "probfd_cli/merge_and_shrink/merge_tree_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "downward/utils/logging.h"

using namespace language::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<MergeTreeFactory>("PMergeTree");
    category.document_synopsis(
        "This page describes the available merge trees that can be used to "
        "precompute a merge strategy, either for the entire task or a "
        "given "
        "subset of transition systems of a given factored transition "
        "system.\n"
        "Merge trees are typically used in the merge strategy of type "
        "'precomputed', but they can also be used as fallback merge "
        "strategies in "
        "'combined' merge strategies.");
}

} // namespace probfd::cli::merge_and_shrink
