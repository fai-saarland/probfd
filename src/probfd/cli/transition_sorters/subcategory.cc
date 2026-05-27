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

class VDiffSorterFeature : public SharedTypedFeature<FDRTransitionSorter> {
public:
    VDiffSorterFeature()
        : TypedFeature("value_gap_sort")
    {
        add_optional_argument_with_default<bool>("prefer_large_gaps", "false");
    }

    std::shared_ptr<FDRTransitionSorter>
    create_component(const Options& opts, const Context&) const override
    {
        return std::make_shared<VDiffSorter>(
            opts.get<bool>("prefer_large_gaps"));
    }
};
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
    n.insert_feature_plugin<VDiffSorterFeature>();
}

} // namespace probfd::cli::transiton_sorters
