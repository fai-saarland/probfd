#include "downward/cli/pdbs/pattern_collection_generator_category.h"

#include "downward/pdbs/pattern_generator.h"

#include "language/ast/internal_type_declaration.h"

using namespace downward::pdbs;

using namespace language::parser;

namespace downward::cli::pdbs {

void add_pattern_collection_generator_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<PatternCollectionGenerator>(
        nspace,
        "PatternCollectionGenerator",
        "Factory for pattern collections");
}

} // namespace downward::cli::pdbs
