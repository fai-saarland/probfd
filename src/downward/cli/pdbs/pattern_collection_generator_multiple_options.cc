#include "downward/cli/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"

#include "language/plugins/internal_function_definition.h"

#include "downward/cli/utils/rng_options.h"

using namespace std;

using downward::cli::utils::add_rng_options_to_feature;

namespace downward::cli::pdbs {

void add_multiple_algorithm_implementation_notes_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature)
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

std::size_t add_multiple_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
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
    const auto n = add_rng_options_to_feature(feature, start_index + 7);
    const auto n2 =
        add_generator_options_to_feature(feature, start_index + n + 7);

    return start_index + n + n2 + 7;
}

} // namespace downward::cli::pdbs
