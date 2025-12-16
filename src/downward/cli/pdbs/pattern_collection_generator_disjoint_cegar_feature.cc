#include "downward/cli/pdbs/pattern_collection_generator_disjoint_cegar_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/cli/pdbs/cegar_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/pdbs/pattern_collection_generator_disjoint_cegar.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;

using namespace language::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_disjoint_cegar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "disjoint_cegar",
        &language::plugins::construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorDisjointCegar,
            int,
            int,
            FSeconds,
            bool,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);
    f.document_title("Disjoint CEGAR");
    f.document_synopsis(
        "This pattern collection generator uses the CEGAR algorithm to "
        "compute a pattern for the planning task. See below "
        "for a description of the algorithm and some implementation notes. "
        "The original algorithm (called single CEGAR) is described in the "
        "paper " +
        get_rovner_et_al_reference());

    add_cegar_implementation_notes_to_feature(f);

    // TODO: these options could be move to the base class; see issue1022.
    f.make_optional_argument_with_default(
        0,
        "max_pdb_size",
        "1000000",
        "maximum number of states per pattern database (ignored for the "
        "initial collection consisting of a singleton pattern for each "
        "goal "
        "variable)");
    f.make_optional_argument_with_default(
        1,
        "max_collection_size",
        "10000000",
        "maximum number of states in the pattern collection (ignored for "
        "the "
        "initial collection consisting of a singleton pattern for each "
        "goal "
        "variable)");
    f.make_optional_argument_with_default(
        2,
        "max_time",
        "seconds_max()",
        "maximum time in seconds for this pattern collection generator "
        "(ignored for computing the initial collection consisting of a "
        "singleton pattern for each goal variable)");
    const auto n = add_cegar_wildcard_option_to_feature(f, 3);
    const auto n2 = add_rng_options_to_feature(f, n + 3);
    add_generator_options_to_feature(f, n + n2 + 3);

    return f;
}

} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_disjoint_cegar_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_disjoint_cegar_to_namespace(n);
}

} // namespace downward::cli::pdbs
