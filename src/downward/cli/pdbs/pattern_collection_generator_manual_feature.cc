#include "downward/cli/pdbs/pattern_collection_generator_manual_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::plugins;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_manual_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "manual_patterns",
        &language::plugins::construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorManual,
            std::vector<Pattern>,
            Verbosity>);
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
