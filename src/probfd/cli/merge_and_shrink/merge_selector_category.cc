#include "probfd/cli/merge_and_shrink/merge_selector_category.h"

#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_selector_factory.h"

using namespace language::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_selector_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<MergeSelectorFactory>(
        "PMergeSelectorFactory",
        "This page describes the available factories for merge selectors. "
        "Merge selectors are used to compute the next merge purely based on "
        "the state of the given factored transition system. "
        "They are used in the merge strategy of type 'stateless', but they can "
        "also easily be used in different 'combined' merged strategies.");
}

} // namespace probfd::cli::merge_and_shrink
