#include "downward_plugins/plugins/plugin.h"

#include "downward/merge_and_shrink/merge_selector.h"

using namespace std;
using namespace merge_and_shrink;

using namespace downward_plugins::plugins;

namespace {

class MergeSelectorCategoryPlugin : public TypedCategoryPlugin<MergeSelector> {
public:
    MergeSelectorCategoryPlugin()
        : TypedCategoryPlugin("MergeSelector")
    {
        document_synopsis(
            "This page describes the available merge selectors. They are used "
            "to "
            "compute the next merge purely based on the state of the given "
            "factored "
            "transition system. They are used in the merge strategy of type "
            "'stateless', but they can also easily be used in different "
            "'combined' "
            "merged strategies.");
    }
} _category_plugin;

} // namespace
