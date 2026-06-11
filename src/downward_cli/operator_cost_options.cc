#include "downward_cli/operator_cost_options.h"

#include "language/plugins/plugin.h"

#include "downward/operator_cost.h"

using namespace std;

using namespace language;
using namespace language::plugins;

namespace downward::cli {

void add_cost_type_options_to_feature(Feature& feature)
{
    feature.add_option<OperatorCost>(
        "cost_type",
        "Operator cost adjustment type. "
        "No matter what this setting is, axioms will always be considered "
        "as actions of cost 0 by the heuristics that treat axioms as actions.",
        "normal");
}

tuple<OperatorCost> get_cost_type_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return make_tuple(opts.get<OperatorCost>(context, "cost_type"));
}

TypedEnumPlugin<OperatorCost> _enum_plugin(
    {{"normal", "all actions are accounted for with their real cost"},
     {"one", "all actions are accounted for as unit cost"},
     {"plusone",
      "all actions are accounted for as their real cost + 1 "
      "(except if all actions have original cost 1, "
      "in which case cost 1 is used). "
      "This is the behaviour known for the heuristics of the LAMA "
      "planner. "
      "This is intended to be used by the heuristics, not search "
      "algorithms, "
      "but is supported for both."}});

} // namespace downward::cli