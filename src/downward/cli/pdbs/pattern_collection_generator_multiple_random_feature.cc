#include "downward/cli/pdbs/pattern_collection_generator_multiple_random_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward/cli/pdbs/random_pattern_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pdbs/pattern_collection_generator_multiple_random.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {

Feature& add_pattern_collection_generator_random_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "random_patterns",
        &downward::cli::plugins::make_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorMultipleRandom,
            bool,
            int,
            int,
            FSeconds,
            FSeconds,
            FSeconds,
            double,
            bool,
            int,
            Verbosity>);
    f.document_title("Multiple Random Patterns");
    f.document_synopsis(
        "This pattern collection generator implements the 'multiple "
        "randomized causal graph' (mRCG) algorithm described in "
        "experiments of the paper" +
        get_rovner_et_al_reference() +
        "It is an instantiation of the 'multiple algorithm framework'. "
        "To compute a pattern in each iteration, it uses the random "
        "pattern algorithm, called 'single randomized causal graph' (sRCG) "
        "in the paper. See below for descriptions of the algorithms.");

    add_random_pattern_implementation_notes_to_feature(f);
    add_multiple_algorithm_implementation_notes_to_feature(f);

    const auto n = add_random_pattern_bidirectional_option_to_feature(f, 0);
    add_multiple_options_to_feature(f, n);

    return f;
}

} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_multiple_random_feature(
    Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_random_to_namespace(n);
}

} // namespace downward::cli::pdbs
