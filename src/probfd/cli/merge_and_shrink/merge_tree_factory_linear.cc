#include "probfd/cli/merge_and_shrink/merge_tree_factory_linear.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/cli/merge_and_shrink/merge_tree_factory_options.h"

#include "probfd/merge_and_shrink/merge_tree_factory_linear.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_tree.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {
class MergeTreeFactoryLinearFeature
    : public SharedTypedFeature<
          MergeTreeFactory,
          int,
          UpdateOption,
          variable_order_finder::VariableOrderType> {
public:
    MergeTreeFactoryLinearFeature()
        : TypedFeature("plinear", &MergeTreeFactoryLinearFeature::func)
    {
        document_title("Linear merge trees");
        document_synopsis(
            "These merge trees implement several linear merge orders, which "
            "are described in the paper:" +
            utils::format_conference_reference(
                {"Malte Helmert", "Patrik Haslum", "Joerg Hoffmann"},
                "Flexible Abstraction Heuristics for Optimal Sequential "
                "Planning",
                "https://ai.dmi.unibas.ch/papers/helmert-et-al-icaps2007.pdf",
                "Proceedings of the Seventeenth International Conference on"
                " Automated Planning and Scheduling (ICAPS 2007)",
                "176-183",
                "AAAI Press",
                "2007"));

        const auto n = add_merge_tree_factory_options_to_feature(*this, 0);

        make_optional_argument_with_default(
            n,
            "variable_order",
            "cg_goal_level",
            "the order in which atomic transition systems are merged");
    }

protected:
    static shared_ptr<MergeTreeFactory> func(
        const utils::Context&,
        int random_seed,
        UpdateOption update_option,
        downward::variable_order_finder::VariableOrderType variable_order)
    {
        return make_shared_from_arg_tuples<MergeTreeFactoryLinear>(
            random_seed,
            update_option,
            variable_order);
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<MergeTreeFactoryLinearFeature>();
}

} // namespace probfd::cli::merge_and_shrink
