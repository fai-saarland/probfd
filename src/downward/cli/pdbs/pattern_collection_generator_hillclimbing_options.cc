#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace utils;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace downward::cli::pdbs {

void add_hillclimbing_options_to_feature(Feature& feature)
{
    feature.document_note(
        "Note",
        "The pattern collection created by the algorithm will always contain "
        "all patterns consisting of a single goal variable, even if this "
        "violates the pdb_max_size or collection_max_size limits.");
    feature.document_note(
        "Note",
        "This pattern generation method generates patterns optimized "
        "for use with the canonical pattern database heuristic.");
    feature.document_note(
        "Implementation Notes",
        "The following will very briefly describe the algorithm and explain "
        "the differences between the original implementation from 2007 and the "
        "new one in Fast Downward.\n\n"
        "The aim of the algorithm is to output a pattern collection for which "
        "the Heuristic#Canonical_PDB yields the best heuristic estimates.\n\n"
        "The algorithm is basically a local search (hill climbing) which "
        "searches the \"pattern neighbourhood\" (starting initially with a "
        "pattern for each goal variable) for improving the pattern collection. "
        "This is done as described in the section \"pattern construction as "
        "search\" in the paper, except for the corrected search "
        "neighbourhood discussed below. For evaluating the "
        "neighbourhood, the \"counting approximation\" as introduced in the "
        "paper was implemented. An important difference however consists in "
        "the fact that this implementation computes all pattern databases for "
        "each candidate pattern rather than using A* search to compute the "
        "heuristic values only for the sample states for each pattern.\n\n"
        "Also the logic for sampling the search space differs a bit from the "
        "original implementation. The original implementation uses a random "
        "walk of a length which is binomially distributed with the mean at the "
        "estimated solution depth (estimation is done with the current pattern "
        "collection heuristic). In the Fast Downward implementation, also a "
        "random walk is used, where the length is the estimation of the number "
        "of solution steps, which is calculated by dividing the current "
        "heuristic estimate for the initial state by the average operator "
        "costs of the planning task (calculated only once and not updated "
        "during sampling!) to take non-unit cost problems into account. This "
        "yields a random walk of an expected lenght of np = 2 * estimated "
        "number of solution steps. If the random walk gets stuck, it is being "
        "restarted from the initial state, exactly as described in the "
        "original paper.\n\n"
        "The section \"avoiding redundant evaluations\" describes how the "
        "search neighbourhood of patterns can be restricted to variables that "
        "are relevant to the variables already included in the pattern by "
        "analyzing causal graphs. There is a mistake in the paper that leads "
        "to some relevant neighbouring patterns being ignored. See the [errata "
        "https://ai.dmi.unibas.ch/research/publications.html] for details. "
        "This "
        "mistake has been addressed in this implementation. "
        "The second approach described in the paper (statistical confidence "
        "interval) is not applicable to this implementation, as it doesn't use "
        "A* search but constructs the entire pattern databases for all "
        "candidate patterns anyway.\n"
        "The search is ended if there is no more improvement (or the "
        "improvement is smaller than the minimal improvement which can be set "
        "as an option), however there is no limit of iterations of the local "
        "search. This is similar to the techniques used in the original "
        "implementation as described in the paper.",
        true);

    feature.add_option<int>(
        "pdb_max_size",
        "maximal number of states per pattern database ",
        "2000000",
        Bounds("1", "infinity"));
    feature.add_option<int>(
        "collection_max_size",
        "maximal number of states in the pattern collection",
        "20000000",
        Bounds("1", "infinity"));
    feature.add_option<int>(
        "num_samples",
        "number of samples (random states) on which to evaluate each "
        "candidate pattern collection",
        "1000",
        Bounds("1", "infinity"));
    feature.add_option<int>(
        "min_improvement",
        "minimum number of samples on which a candidate pattern "
        "collection must improve on the current one to be considered "
        "as the next pattern collection ",
        "10",
        Bounds("1", "infinity"));
    feature.add_option<double>(
        "max_time",
        "maximum time in seconds for improving the initial pattern "
        "collection via hill climbing. If set to 0, no hill climbing "
        "is performed at all. Note that this limit only affects hill "
        "climbing. Use max_time_dominance_pruning to limit the time "
        "spent for pruning dominated patterns.",
        "infinity",
        Bounds("0.0", "infinity"));
    add_rng_options_to_feature(feature);
}

tuple<int, int, int, int, double, int>
get_hillclimbing_arguments_from_options(const Options& opts)
{
    return tuple_cat(
        make_tuple(
            opts.get<int>("pdb_max_size"),
            opts.get<int>("collection_max_size"),
            opts.get<int>("num_samples"),
            opts.get<int>("min_improvement"),
            opts.get<double>("max_time")),
        get_rng_arguments_from_options(opts));
}

} // namespace downward::cli::pdbs
