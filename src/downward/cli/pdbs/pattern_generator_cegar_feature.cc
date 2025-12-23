#include "downward/cli/pdbs/pattern_generator_cegar_feature.h"

#include "downward/cli/pdbs/cegar_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/pdbs/utils.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/pdbs/pattern_generator_cegar.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_generator_cegar_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "cegar_pattern",
        &language::plugins::construct_shared<
            PatternGenerator,
            PatternGeneratorCEGAR,
            int,
            FSeconds,
            bool,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);
    f.document_title("CEGAR");
    f.document_synopsis(
        "This pattern generator uses the CEGAR algorithm restricted to a "
        "random single goal of the task to compute a pattern. See below "
        "for a description of the algorithm and some implementation notes. "
        "The original algorithm (called single CEGAR) is described in the "
        "paper " +
        get_rovner_et_al_reference());

    add_cegar_implementation_notes_to_feature(f);

    f.make_optional_argument_with_default(
        0,
        "max_pdb_size",
        "1000000",
        "maximum number of states in the final pattern database (possibly "
        "ignored by a singleton pattern consisting of a single goal "
        "variable)");
    f.make_optional_argument_with_default(
        1,
        "max_time",
        "seconds_max()",
        "maximum time in seconds for the pattern generation");
    const auto n = add_cegar_wildcard_option_to_feature(f, 2);
    const auto n2 = add_rng_options_to_feature(f, n + 2);
    add_generator_options_to_feature(f, n + n2 + 2);

    return f;
}

} // namespace downward::cli::pdbs
