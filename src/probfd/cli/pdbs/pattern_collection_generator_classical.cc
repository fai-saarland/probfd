#include "probfd/cli/pdbs/pattern_collection_generator_classical.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

#include "probfd/pdbs/pattern_collection_generator_classical.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;
using namespace probfd::cli::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_classical_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "classical_generator",
        &construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorClassical,
            std::shared_ptr<downward::pdbs::PatternCollectionGenerator>,
            std::shared_ptr<SubCollectionFinderFactory>,
            Verbosity>);

    f.document_title("Classical Pattern Generation Adapter");
    f.document_synopsis(
        "Uses a classical pattern generation method on the determinization "
        "of the input task. If classical PDBs are constructed by the "
        "generation algorithm, they are used as a heuristic to compute the "
        "corresponding probability-aware PDBs.");

    f.make_optional_argument_with_default(
        0,
        "generator",
        "systematic()",
        "The classical pattern collection generator.");

    f.make_optional_argument_with_default(
        1,
        "subcollection_finder_factory",
        "finder_trivial_factory()",
        "The subcollection finder factory.");

    add_pattern_collection_generator_options_to_feature(f, 2);

    return f;
}

} // namespace probfd::cli::pdbs
