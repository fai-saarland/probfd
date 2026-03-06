#include "probfd/cli/pdbs/pattern_collection_generator_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/pdbs/pattern_collection_generator.h"

using namespace probfd::pdbs;

using namespace language::parser;

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_category(
    NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<PatternCollectionGenerator>(
        nspace,
        "PPDBPatternCollectionGenerator",
        "Factory for pattern collections and/or "
        "corresponding probability-aware "
        "PDBs");
}

} // namespace probfd::cli::pdbs
