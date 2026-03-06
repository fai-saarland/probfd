#include "downward/cli/pruning/pruning_method_category.h"

#include "downward/pruning_method.h"

#include "language/ast/internal_type_declaration.h"

using namespace std;

namespace downward::cli::pruning {

void add_pruning_method_category(
    language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<PruningMethod>(
        nspace,
        "PruningMethod",
        "Prune or reorder applicable operators.");
}

} // namespace downward::cli::pruning
