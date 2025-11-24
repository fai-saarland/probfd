#include "downward/cli/pdbs/pattern_generator_random_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/pdbs/random_pattern_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/pdbs/pattern_generator_random.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

Feature& add_pattern_generator_random_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "random_pattern",
        &downward::cli::plugins::make_shared<
            PatternGenerator,
            PatternGeneratorRandom,
            int,
            FSeconds,
            bool,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);

    f.document_title("Random Pattern");
    f.document_synopsis(
        "This pattern generator implements the 'single randomized "
        "causal graph' algorithm described in experiments of the the "
        "paper" +
        get_rovner_et_al_reference() +
        "See below for a description of the algorithm and some "
        "implementation "
        "notes.");

    add_random_pattern_implementation_notes_to_feature(f);

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
    const auto n = add_random_pattern_bidirectional_option_to_feature(f, 2);
    const auto n2 = add_rng_options_to_feature(f, n + 2);
    add_generator_options_to_feature(f, n + n2 + 2);

    return f;
}

} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_random_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_generator_random_to_namespace(n);
}

} // namespace downward::cli::pdbs
