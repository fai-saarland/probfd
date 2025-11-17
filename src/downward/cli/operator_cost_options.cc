#include "downward/cli/operator_cost_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/operator_cost.h"

using namespace std;

namespace downward::cli {

using namespace plugins;

std::size_t add_cost_type_options_to_feature(Feature& feature, std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "cost_type",
        "normal",
        "Operator cost adjustment type. "
        "No matter what this setting is, axioms will always be considered "
        "as actions of cost 0 by the heuristics that treat axioms as "
        "actions.");
    return 1;
}

} // namespace downward::cli