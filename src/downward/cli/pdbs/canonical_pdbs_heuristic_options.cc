#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"

using namespace std;
using namespace downward::pdbs;

namespace downward::cli::pdbs {

void add_canonical_pdbs_options_to_feature(plugins::Feature& feature)
{
    feature.add_option<double>(
        "max_time_dominance_pruning",
        "The maximum time in seconds spent on dominance pruning. Using 0.0 "
        "turns off dominance pruning. Dominance pruning excludes patterns "
        "and additive subsets that will never contribute to the heuristic "
        "value because there are dominating subsets in the collection.",
        "infinity",
        plugins::Bounds("0.0", "infinity"));
}

tuple<double>
get_canonical_pdbs_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple(opts.get<double>("max_time_dominance_pruning"));
}

} // namespace downward::cli::pdbs
