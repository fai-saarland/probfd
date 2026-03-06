#include "probfd/cli/merge_and_shrink/prune_strategy_solvable.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/merge_and_shrink/prune_strategy_solvable.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace language::parser;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_prune_strategy_solvable_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        PruneStrategy,
        PruneStrategySolvable>>(nspace, "prune_solvable");

    f.document_title("Solvable states prune strategy");
    f.document_synopsis("This prune strategy keeps only solvable states.");

    return f;
}

} // namespace probfd::cli::merge_and_shrink