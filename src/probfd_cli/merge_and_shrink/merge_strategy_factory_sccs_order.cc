#include "probfd_cli/merge_and_shrink/merge_strategy_factory_sccs_order.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_sccs_order.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

using namespace std;
using namespace downward;
using namespace probfd::merge_and_shrink;

using namespace language;
using namespace language::plugins;

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_sccs_order(RawRegistry& raw_registry)
{
    raw_registry.insert_enum_plugin<OrderOfSCCs>(
        {{"topological",
          "according to the topological ordering of the directed graph "
          "where each obtained SCC is a 'supervertex'"},
         {"reverse_topological",
          "according to the reverse topological ordering of the directed "
          "graph where each obtained SCC is a 'supervertex'"},
         {"decreasing",
          "biggest SCCs first, using 'topological' as tie-breaker"},
         {"increasing",
          "smallest SCCs first, using 'topological' as tie-breaker"}});
}

} // namespace probfd::cli::merge_and_shrink
