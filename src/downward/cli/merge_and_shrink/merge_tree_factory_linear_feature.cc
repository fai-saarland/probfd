#include "downward/cli/merge_and_shrink/merge_tree_factory_linear_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/merge_and_shrink/merge_tree_options.h"

#include "downward/merge_and_shrink/merge_tree_factory_linear.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

using downward::cli::merge_and_shrink::add_merge_tree_options_to_feature;
using downward::cli::merge_and_shrink::get_merge_tree_arguments_from_options;

namespace {
class MergeTreeFactoryLinearFeature
    : public TypedFeature<MergeTreeFactory, MergeTreeFactoryLinear> {
public:
    MergeTreeFactoryLinearFeature()
        : TypedFeature("linear")
    {
        document_title("Linear merge trees");
        document_synopsis(
            "These merge trees implement several linear merge orders, which "
            "are described in the paper:" +
            format_conference_reference(
                {"Malte Helmert", "Patrik Haslum", "Joerg Hoffmann"},
                "Flexible Abstraction Heuristics for Optimal Sequential "
                "Planning",
                "https://ai.dmi.unibas.ch/papers/helmert-et-al-icaps2007.pdf",
                "Proceedings of the Seventeenth International Conference on"
                " Automated Planning and Scheduling (ICAPS 2007)",
                "176-183",
                "AAAI Press",
                "2007"));

        add_option<downward::variable_order_finder::VariableOrderType>(
            "variable_order",
            "the order in which atomic transition systems are merged",
            "cg_goal_level");

        add_merge_tree_options_to_feature(*this);
    }

    virtual shared_ptr<MergeTreeFactoryLinear>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<MergeTreeFactoryLinear>(
            opts.get<downward::variable_order_finder::VariableOrderType>(
                "variable_order"),
            get_merge_tree_arguments_from_options(opts));
    }
};
}

namespace downward::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<MergeTreeFactoryLinearFeature>();

    raw_registry.insert_enum_plugin<
        downward::variable_order_finder::VariableOrderType>(
        {{"cg_goal_level",
          "variables are prioritized first if they have an arc to a previously "
          "added variable, second if their goal value is defined "
          "and third according to their level in the causal graph"},
         {"cg_goal_random",
          "variables are prioritized first if they have an arc to a previously "
          "added variable, second if their goal value is defined "
          "and third randomly"},
         {"goal_cg_level",
          "variables are prioritized first if their goal value is defined, "
          "second if they have an arc to a previously added variable, "
          "and third according to their level in the causal graph"},
         {"random", "variables are ordered randomly"},
         {"level",
          "variables are ordered according to their level in the causal graph"},
         {"reverse_level",
          "variables are ordered reverse to their level in the causal graph"}});
}

} // namespace
