#include "downward/cli/pdbs/pattern_collection_generator_combo_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_combo.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace language::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_combo_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "combo",
        &language::plugins::construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorCombo,
            int,
            Verbosity>);
    f.make_optional_argument_with_default(
        0,
        "max_states",
        "1000000",
        "maximum abstraction size for combo strategy");
    add_generator_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::pdbs
