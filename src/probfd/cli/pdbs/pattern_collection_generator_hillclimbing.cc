#include "probfd/cli/pdbs/pattern_collection_generator_hillclimbing.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

#include "probfd/pdbs/pattern_collection_generator_hillclimbing.h"

using namespace downward;
using namespace utils;

using namespace probfd::cli::pdbs;
using namespace probfd::pdbs;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

Feature&
add_pattern_collection_generator_hill_climbing_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "hillclimbing_probabilistic",
        &downward::cli::plugins::make_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorHillclimbing,
            std::shared_ptr<PatternCollectionGenerator>,
            std::shared_ptr<SubCollectionFinderFactory>,
            int,
            int,
            int,
            int,
            FSeconds,
            int,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);

    f.make_optional_argument_with_default(
        0,
        "initial_generator",
        "psystematic(pattern_max_size=1)",
        "generator for the initial pattern database");

    f.make_optional_argument_with_default(
        1,
        "subcollection_finder_factory",
        "finder_trivial_factory()",
        "The subcollection finder factory.");

    f.make_optional_argument_with_default(
        2,
        "pdb_max_size",
        "2M",
        "maximal number of states per pattern database");

    f.make_optional_argument_with_default(
        3,
        "collection_max_size",
        "10M",
        "maximal number of states in the pattern collection");

    f.make_optional_argument_with_default(
        4,
        "search_space_max_size",
        "30M",
        "maximal number of states in the pattern search space");

    f.make_optional_argument_with_default(
        5,
        "num_samples",
        "1000",
        "number of samples (random states) on which to evaluate each "
        "candidate pattern collection");

    f.make_optional_argument_with_default(
        6,
        "min_improvement",
        "10",
        "minimum number of samples on which a candidate pattern "
        "collection must improve on the current one to be considered "
        "as the next pattern collection");

    f.make_optional_argument_with_default(
        7,
        "max_time",
        "seconds_max()",
        "maximum time in seconds for improving the initial pattern "
        "collection via hill climbing. If set to 0, no hill climbing "
        "is performed at all. Note that this limit only affects hill "
        "climbing. Use max_time_dominance_pruning to limit the time "
        "spent for pruning dominated patterns.");

    const auto n = add_rng_options_to_feature(f, 7);
    add_pattern_collection_generator_options_to_feature(f, n);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_hillclimbing_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_hill_climbing_to_namespace(n);
}

} // namespace probfd::cli::pdbs
