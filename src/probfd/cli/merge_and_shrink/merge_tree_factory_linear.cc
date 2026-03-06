#include "probfd/cli/merge_and_shrink/merge_tree_factory_linear.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/cli/merge_and_shrink/merge_tree_factory_options.h"

#include "probfd/merge_and_shrink/merge_tree_factory_linear.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_tree.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace language::parser;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_merge_tree_factory_linear_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        MergeTreeFactory,
        MergeTreeFactoryLinear,
        std::shared_ptr<utils::RandomNumberGenerator>,
        UpdateOption,
        variable_order_finder::VariableOrderType>>(nspace, "plinear");

    f.document_title("Linear merge trees");
    f.document_synopsis(
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

    const auto n = add_merge_tree_factory_options_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "variable_order",
        "cg_goal_level",
        "the order in which atomic transition systems are merged");

    return f;
}

} // namespace probfd::cli::merge_and_shrink
