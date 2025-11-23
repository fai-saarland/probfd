#include "downward/cli/operator_cost_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/operator_cost.h"

using namespace std;

namespace downward::cli {

using namespace plugins;

void add_operator_cost_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<OperatorCost>(
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
}

} // namespace downward::cli