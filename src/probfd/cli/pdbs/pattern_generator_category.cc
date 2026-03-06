#include "probfd/cli/pdbs/pattern_generator_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/pdbs/pattern_generator.h"

using namespace probfd::pdbs;

using namespace language::parser;

namespace probfd::cli::pdbs {

void add_pattern_generator_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<PatternGenerator>(
        nspace,
        "PPDBPatternGenerator",
        "Factory for a pattern and/or the corresponding "
        "probability-aware PDB");
}

} // namespace probfd::cli::pdbs
