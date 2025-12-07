#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"

#include "language/plugins/plugin.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"

using namespace std;
using namespace downward::pdbs;

namespace downward::cli::pdbs {

std::size_t add_canonical_pdbs_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "max_time_dominance_pruning",
        "seconds_max()",
        "The maximum time in seconds spent on dominance pruning. Using 0.0 "
        "turns off dominance pruning. Dominance pruning excludes patterns "
        "and additive subsets that will never contribute to the heuristic "
        "value because there are dominating subsets in the collection.");

    return 1;
}

} // namespace downward::cli::pdbs
