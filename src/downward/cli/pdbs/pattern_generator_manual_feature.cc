#include "downward/cli/pdbs/pattern_generator_manual_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/pdbs/pattern_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::plugins;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_generator_manual_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "manual_pattern",
        &language::plugins::construct_shared<
            PatternGenerator,
            PatternGeneratorManual,
            std::vector<int>,
            Verbosity>);
    f.make_required_argument(
        0,
        "pattern",
        "list of variable numbers of the planning task that should be used "
        "as the pattern.");
    add_generator_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::pdbs
