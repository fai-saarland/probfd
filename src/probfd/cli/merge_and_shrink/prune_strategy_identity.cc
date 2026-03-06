#include "probfd/cli/merge_and_shrink/prune_strategy_identity.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/merge_and_shrink/prune_strategy_identity.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace language::parser;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_prune_strategy_identity_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        PruneStrategy,
        PruneStrategyIdentity>>(nspace, "prune_identity");

    f.document_title("Identity prune strategy");
    f.document_synopsis("This prune strategy leaves the TS unchanged.");

    return f;
}

} // namespace probfd::cli::merge_and_shrink