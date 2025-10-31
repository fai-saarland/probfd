#include "probfd/cli/pdbs/pattern_collection_generator_multiple_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

#include "downward/utils/rng_options.h"

using namespace std;
using namespace downward;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace probfd::cli::pdbs {

void add_multiple_options_to_feature(Feature& feature)
{
    feature.add_optional_argument_with_default<int>(
        "max_pdb_size",
        "1M",
        "maximum number of states for each pattern database, computed "
        "by compute_pattern (possibly ignored by singleton patterns consisting "
        "of a goal variable)");
    feature.add_optional_argument_with_default<int>(
        "max_collection_size",
        "10M",
        "maximum number of states in all pattern databases of the "
        "collection (possibly ignored, see max_pdb_size)");
    feature.add_optional_argument_with_default<double>(
        "pattern_generation_max_time",
        "infinity",
        "maximum time in seconds for each call to the algorithm for "
        "computing a single pattern");
    feature.add_optional_argument_with_default<double>(
        "total_max_time",
        "100.0",
        "maximum time in seconds for this pattern collection generator. "
        "It will always execute at least one iteration, i.e., call the "
        "algorithm for computing a single pattern at least once.");
    feature.add_optional_argument_with_default<double>(
        "stagnation_limit",
        "20.0",
        "maximum time in seconds this pattern generator is allowed to run "
        "without generating a new pattern. It terminates prematurely if this "
        "limit is hit unless enable_blacklist_on_stagnation is enabled.");
    feature.add_optional_argument_with_default<double>(
        "blacklist_trigger_percentage",
        "0.75",
        "percentage of total_max_time after which blacklisting is enabled");
    feature.add_optional_argument_with_default<bool>(
        "enable_blacklist_on_stagnation",
        "true",
        "if true, blacklisting is enabled when stagnation_limit is hit "
        "for the first time (unless it was already enabled due to "
        "blacklist_trigger_percentage) and pattern generation is terminated "
        "when stagnation_limit is hit for the second time. If false, pattern "
        "generation is terminated already the first time stagnation_limit is "
        "hit.");
    feature.add_optional_argument_with_default<bool>(
        "use_saturated_costs",
        "true",
        "if true, saturated cost partitioning is used to combine the generated "
        "patterns. In each iteration, costs are decreased by the minimum "
        "saturated cost function of the most recently constructed PDB and the "
        "algorithm continues with the remaining costs. If false, the maximum "
        "PDB estimate is used.");
    add_rng_options_to_feature(feature);
    add_pattern_collection_generator_options_to_feature(feature);
}

PatternCollectionGeneratorMultipleArgs
get_multiple_arguments_from_options(const Options& options)
{
    return std::tuple_cat(
        std::make_tuple(
            options.get<int>("max_pdb_size"),
            options.get<int>("max_collection_size"),
            options.get<utils::FSeconds>("pattern_generation_max_time"),
            options.get<utils::FSeconds>("total_max_time"),
            options.get<utils::FSeconds>("stagnation_limit"),
            options.get<double>("blacklist_trigger_percentage"),
            options.get<bool>("enable_blacklist_on_stagnation"),
            options.get<bool>("use_saturated_costs"),
            utils::get_rng(
                std::get<0>(get_rng_arguments_from_options(options)))),
        get_pattern_collection_generator_arguments_from_options(options));
}

} // namespace probfd::cli::pdbs
