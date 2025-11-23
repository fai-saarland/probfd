#include "probfd/cli/transition_sorters/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/transition_sorters/vdiff_sorter.h"

using namespace downward;
using namespace downward::utils;

using namespace probfd;
using namespace probfd::transition_sorters;

using namespace downward::cli::plugins;

namespace {

Feature& add_vdiff_sorter_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "value_gap_sort",
        &downward::cli::plugins::
            make_shared<FDRTransitionSorter, VDiffSorter, bool>);
    f.make_optional_argument_with_default(0, "prefer_large_gaps", "false");

    return f;
}

} // namespace

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<FDRTransitionSorter>(
        "FDRTransitionSorter",
        "");
}

void add_transition_sorter_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_vdiff_sorter_to_namespace(n);
}

} // namespace probfd::cli::transiton_sorters
