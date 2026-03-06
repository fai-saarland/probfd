#include "downward/cli/pdbs/pattern_collection_generator_manual_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::parser;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase& add_pattern_collection_generator_manual_feature(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        PatternCollectionGenerator,
        PatternCollectionGeneratorManual,
        std::vector<Pattern>,
        Verbosity>>(nspace, "manual_patterns");

    f.make_required_argument(
        0,
        "patterns",
        "list of patterns (which are lists of variable numbers of the "
        "planning "
        "task).");
    add_generator_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::pdbs
