#include "downward/cli/pdbs/pattern_collection_generator_multiple_cegar_feature.h"
#include "downward/cli/pdbs/cegar_options.h"
#include "downward/cli/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward/cli/pdbs/utils.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_multiple_cegar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "multiple_cegar",
        &downward::cli::plugins::construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorMultipleCegar,
            bool,
            int,
            int,
            FSeconds,
            FSeconds,
            FSeconds,
            double,
            bool,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);

    f.document_title("Multiple CEGAR");
    f.document_synopsis(
        "This pattern collection generator implements the multiple CEGAR "
        "algorithm described in the paper" +
        get_rovner_et_al_reference() +
        "It is an instantiation of the 'multiple algorithm framework'. "
        "To compute a pattern in each iteration, it uses the CEGAR "
        "algorithm "
        "restricted to a single goal variable. See below for descriptions "
        "of "
        "the algorithms.");

    add_cegar_implementation_notes_to_feature(f);
    add_multiple_algorithm_implementation_notes_to_feature(f);

    const auto n = add_cegar_wildcard_option_to_feature(f, 0);
    add_multiple_options_to_feature(f, n);

    return f;
}

} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_multiple_cegar_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_multiple_cegar_to_namespace(n);
}

} // namespace downward::cli::pdbs
