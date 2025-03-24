#include "probfd/merge_and_shrink/merge_selector.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace {

class MergeSelectorCategoryPlugin
    : public TypedCategoryPlugin<MergeSelector> {
public:
    MergeSelectorCategoryPlugin()
        : TypedCategoryPlugin("PMergeSelector")
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

}
