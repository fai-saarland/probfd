#include "downward/cli/merge_and_shrink/merge_tree_factory_linear_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/merge_and_shrink/merge_tree_options.h"

#include "downward/merge_and_shrink/merge_tree_factory_linear.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace language::plugins;

using downward::cli::merge_and_shrink::add_merge_tree_options_to_feature;

namespace {

InternalFunctionDefinitionBase& add_merge_strategy_factory_linear_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "linear",
        &language::plugins::construct_shared<
            MergeTreeFactory,
            MergeTreeFactoryLinear,
            downward::variable_order_finder::VariableOrderType,
            std::shared_ptr<RandomNumberGenerator>,
            UpdateOption>);

    f.document_title("Linear merge trees");
    f.document_synopsis(
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

    f.make_optional_argument_with_default(
        0,
        "variable_order",
        "cg_goal_level",
        "the order in which atomic transition systems are merged");

    add_merge_tree_options_to_feature(f, 1);

    return f;
}

} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<downward::variable_order_finder::VariableOrderType>(
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

    add_merge_strategy_factory_linear_to_namespace(n);
}

} // namespace downward::cli::merge_and_shrink
