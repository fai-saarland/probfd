#include "downward/cli/plugins/plugin.h"

#include "downward/merge_and_shrink/merge_tree_factory.h"

using namespace std;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace {

class MergeTreeFactoryCategoryPlugin
    : public TypedCategoryPlugin<MergeTreeFactory> {
public:
    MergeTreeFactoryCategoryPlugin()
        : TypedCategoryPlugin("MergeTree")
    {
        document_synopsis(
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
} _category_plugin;

TypedEnumPlugin<UpdateOption> _enum_plugin(
    {{"use_first",
      "the node representing the index that would have been merged earlier "
      "survives"},
     {"use_second",
      "the node representing the index that would have been merged later "
      "survives"},
     {"use_random", "a random node (of the above two) survives"}});

} // namespace
