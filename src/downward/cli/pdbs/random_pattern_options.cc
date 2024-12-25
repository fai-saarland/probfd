#include "downward/cli/pdbs/random_pattern_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;

namespace downward::cli::pdbs {

void add_random_pattern_implementation_notes_to_feature(
    plugins::Feature& feature)
{
    feature.document_note(
        "Short description of the random pattern algorithm",
        "The random pattern algorithm computes a pattern for a given planning "
        "task and a single goal of the task as follows. Starting with the "
        "given "
        "goal variable, the algorithm executes a random walk on the causal "
        "graph. In each iteration, it selects a random causal graph neighbor "
        "of "
        "the current variable. It terminates if no neighbor fits the pattern "
        "due "
        "to the size limit or if the time limit is reached.",
        true);
    feature.document_note(
        "Implementation notes about the random pattern algorithm",
        "In the original implementation used in the paper, the algorithm "
        "selected a random neighbor and then checked if selecting it would "
        "violate the PDB size limit. If so, the algorithm would not select "
        "it and terminate. In the current implementation, the algorithm "
        "instead "
        "loops over all neighbors of the current variable in random order and "
        "selects the first one not violating the PDB size limit. If no such "
        "neighbor exists, the algorithm terminates.",
        true);
}

void add_random_pattern_bidirectional_option_to_feature(
    plugins::Feature& feature)
{
    feature.add_option<bool>(
        "bidirectional",
        "this option decides if the causal graph is considered to be "
        "directed or undirected selecting predecessors of already selected "
        "variables. If true (default), it is considered to be undirected "
        "(precondition-effect edges are bidirectional). If false, it is "
        "considered to be directed (a variable is a neighbor only if it is a "
        "predecessor.",
        "true");
}

tuple<bool> get_random_pattern_bidirectional_arguments_from_options(
    const plugins::Options& opts)
{
    return make_tuple(opts.get<bool>("bidirectional"));
}

} // namespace downward::cli::pdbs
