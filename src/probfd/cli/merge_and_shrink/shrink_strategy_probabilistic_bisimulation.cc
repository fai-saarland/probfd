#include "probfd/cli/merge_and_shrink/shrink_strategy_probabilistic_bisimulation.h"

#include "probfd/merge_and_shrink/shrink_strategy_probabilistic_bisimulation.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

using namespace std;
using namespace downward;
using namespace language::parser;
using namespace probfd::merge_and_shrink;

namespace {

InternalFunctionDefinitionBase&
add_shrink_strategy_probabilistic_bisimulation_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        ShrinkStrategy,
        ShrinkStrategyProbabilisticBisimulation,
        ShrinkStrategyProbabilisticBisimulation::AtLimit,
        bool>>(nspace, "pshrink_probabilistic_bisimulation");

    f.document_title("Probabilistic Bisimulation-based shrink strategy");
    f.document_synopsis(
        "Computes a probabilistic bisimulation over the induced "
        "probabilistic transition system and emits the corresponding "
        "abstraction mapping. This strategy is not exact.");

    f.make_optional_argument_with_default(
        0,
        "at_limit",
        "return",
        "what to do when the size limit is hit");

    f.make_optional_argument_with_default(
        1,
        "require_goal_distances",
        "true",
        "whether goal distances are required");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_probabilistic_bisimulation_feature(
    NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<ShrinkStrategyProbabilisticBisimulation::AtLimit>(
        nspace,
        "AtLimit",
        {{"return", "stop without refining the equivalence class further"},
         {"use_up",
          "continue refining the equivalence class until "
          "the size limit is hit"}});

    add_shrink_strategy_probabilistic_bisimulation_to_namespace(nspace);
}

} // namespace probfd::cli::merge_and_shrink
