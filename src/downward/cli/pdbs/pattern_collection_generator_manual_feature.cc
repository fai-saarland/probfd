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

namespace {

InternalFunctionDefinitionBase& add_pattern_collection_generator_manual_to_namespace(Namespace& nspace)
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

} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_manual_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_manual_to_namespace(n);
}

} // namespace downward::cli::pdbs
