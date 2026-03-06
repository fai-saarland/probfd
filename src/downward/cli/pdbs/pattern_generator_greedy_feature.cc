#include "downward/cli/pdbs/pattern_generator_greedy_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "language/ast/internal_function_definition.h"

#include "downward/pdbs/pattern_generator_greedy.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::parser;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_generator_greedy_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        PatternGenerator,
        PatternGeneratorGreedy,
        int,
        Verbosity>>(nspace, "greedy");

    f.make_optional_argument_with_default(
        0,
        "max_states",
        "1000000",
        "maximal number of abstract states in the pattern database.");
    add_generator_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::pdbs
