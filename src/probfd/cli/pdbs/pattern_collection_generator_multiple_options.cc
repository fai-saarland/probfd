#include "probfd/cli/pdbs/pattern_collection_generator_multiple_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

using namespace std;
using namespace downward;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace probfd::cli::pdbs {

std::size_t
add_multiple_options_to_feature(Feature& feature, std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "max_pdb_size",
        "1M",
        "maximum number of states for each pattern database, computed "
        "by compute_pattern (possibly ignored by singleton patterns consisting "
        "of a goal variable)");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "max_collection_size",
        "10M",
        "maximum number of states in all pattern databases of the "
        "collection (possibly ignored, see max_pdb_size)");
    feature.make_optional_argument_with_default(
        start_index + 2,
        "pattern_generation_max_time",
        "seconds_max()",
        "maximum time in seconds for each call to the algorithm for "
        "computing a single pattern");
    feature.make_optional_argument_with_default(
        start_index + 3,
        "total_max_time",
        "100.0s",
        "maximum time in seconds for this pattern collection generator. "
        "It will always execute at least one iteration, i.e., call the "
        "algorithm for computing a single pattern at least once.");
    feature.make_optional_argument_with_default(
        start_index + 4,
        "stagnation_limit",
        "20.0s",
        "maximum time in seconds this pattern generator is allowed to run "
        "without generating a new pattern. It terminates prematurely if this "
        "limit is hit unless enable_blacklist_on_stagnation is enabled.");
    feature.make_optional_argument_with_default(
        start_index + 5,
        "blacklist_trigger_percentage",
        "0.75",
        "percentage of total_max_time after which blacklisting is enabled");
    feature.make_optional_argument_with_default(
        start_index + 6,
        "enable_blacklist_on_stagnation",
        "true",
        "if true, blacklisting is enabled when stagnation_limit is hit "
        "for the first time (unless it was already enabled due to "
        "blacklist_trigger_percentage) and pattern generation is terminated "
        "when stagnation_limit is hit for the second time. If false, pattern "
        "generation is terminated already the first time stagnation_limit is "
        "hit.");
    feature.make_optional_argument_with_default(
        start_index + 7,
        "use_saturated_costs",
        "true",
        "if true, saturated cost partitioning is used to combine the generated "
        "patterns. In each iteration, costs are decreased by the minimum "
        "saturated cost function of the most recently constructed PDB and the "
        "algorithm continues with the remaining costs. If false, the maximum "
        "PDB estimate is used.");
    const auto n = add_rng_options_to_feature(feature, start_index + 8);
    const auto n2 = add_pattern_collection_generator_options_to_feature(
        feature,
        n + start_index + 8);

    return n + n2 + 8;
}

} // namespace probfd::cli::pdbs
