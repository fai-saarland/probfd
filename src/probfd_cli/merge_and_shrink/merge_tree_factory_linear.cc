#include "probfd_cli/merge_and_shrink/merge_tree_factory_linear.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd_cli/merge_and_shrink/merge_tree_factory_options.h"

#include "probfd/merge_and_shrink/merge_tree_factory_linear.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_tree.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

using namespace language;
using namespace language::plugins;

namespace {
class MergeTreeFactoryLinearFeature : public TypedFeature<MergeTreeFactory> {
public:
    MergeTreeFactoryLinearFeature()
        : TypedFeature("plinear")
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

        add_merge_tree_factory_options_to_feature(*this);

        add_option<variable_order_finder::VariableOrderType>(
            "variable_order",
            "the order in which atomic transition systems are merged",
            "cg_goal_level");
    }

protected:
    shared_ptr<MergeTreeFactory>
    create_component(const Options& options, const Context& context)
        const override
    {
        return make_shared_from_arg_tuples<MergeTreeFactoryLinear>(
            get_merge_tree_factory_args_from_options(context, options),
            options.get<variable_order_finder::VariableOrderType>(
                context,
                "variable_order"));
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<MergeTreeFactoryLinearFeature>();
}

} // namespace probfd::cli::merge_and_shrink
