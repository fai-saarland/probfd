#include "probfd/cli/merge_and_shrink/merge_tree_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "probfd/merge_and_shrink/merge_tree.h"

#include "downward/utils/logging.h"

using namespace downward::cli::plugins;
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

    raw_registry.insert_enum_plugin<UpdateOption>(
        {{"use_first",
          "the node representing the index that would have been merged earlier "
          "survives"},
         {"use_second",
          "the node representing the index that would have been merged later "
          "survives"},
         {"use_random", "a random node (of the above two) survives"}});
}

} // namespace probfd::cli::merge_and_shrink
