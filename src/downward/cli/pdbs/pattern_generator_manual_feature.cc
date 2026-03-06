#include "downward/cli/pdbs/pattern_generator_manual_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "language/ast/internal_function_definition.h"

#include "downward/pdbs/pattern_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::parser;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_generator_manual_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        PatternGenerator,
        PatternGeneratorManual,
        std::vector<int>,
        Verbosity>>(nspace, "manual_pattern");

    f.make_required_argument(
        0,
        "pattern",
        "list of variable numbers of the planning task that should be used "
        "as the pattern.");
    add_generator_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::pdbs
