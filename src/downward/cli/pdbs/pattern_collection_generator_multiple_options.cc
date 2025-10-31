#include "downward/cli/plugins/plugin.h"

#include "downward/cli/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/utils/logging.h"

using namespace std;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace downward::cli::pdbs {

void add_multiple_algorithm_implementation_notes_to_feature(
    plugins::Feature& feature)
{
    feature.document_note(
        "Short description of the 'multiple algorithm framework'",
        "This algorithm is a general framework for computing a pattern "
        "collection "
        "for a given planning task. It requires as input a method for "
        "computing a "
        "single pattern for the given task and a single goal of the task. The "
        "algorithm works as follows. It first stores the goals of the task in "
        "random order. Then, it repeatedly iterates over all goals and for "
        "each "
        "goal, it uses the given method for computing a single pattern. If the "
        "pattern is new (duplicate detection), it is kept for the final "
        "collection.\n"
        "The algorithm runs until reaching a given time limit. Another "
        "parameter allows "
        "exiting early if no new patterns are found for a certain time "
        "('stagnation'). "
        "Further parameters allow enabling blacklisting for the given pattern "
        "computation "
        "method after a certain time to force some diversification or to "
        "enable said "
        "blacklisting when stagnating.",
        true);
    feature.document_note(
        "Implementation note about the 'multiple algorithm framework'",
        "A difference compared to the original implementation used in the "
        "paper is that the original implementation of stagnation in "
        "the multiple CEGAR/RCG algorithms started counting the time towards "
        "stagnation only after having generated a duplicate pattern. Now, "
        "time towards stagnation starts counting from the start and is reset "
        "to the current time only when having found a new pattern or when "
        "enabling blacklisting.",
        true);
}

void add_multiple_options_to_feature(plugins::Feature& feature)
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
    add_rng_options_to_feature(feature);
    add_generator_options_to_feature(feature);
}

tuple<
    int,
    int,
    downward::utils::FSeconds,
    downward::utils::FSeconds,
    downward::utils::FSeconds,
    double,
    bool,
    int,
    downward::utils::Verbosity>
get_multiple_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        make_tuple(
            opts.get<int>("max_pdb_size"),
            opts.get<int>("max_collection_size"),
            opts.get<downward::utils::FSeconds>("pattern_generation_max_time"),
            opts.get<downward::utils::FSeconds>("total_max_time"),
            opts.get<downward::utils::FSeconds>("stagnation_limit"),
            opts.get<double>("blacklist_trigger_percentage"),
            opts.get<bool>("enable_blacklist_on_stagnation")),
        get_rng_arguments_from_options(opts),
        get_generator_arguments_from_options(opts));
}

} // namespace downward::cli::pdbs
