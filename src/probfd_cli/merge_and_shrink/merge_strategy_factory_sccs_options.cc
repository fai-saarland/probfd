#include "probfd_cli/merge_and_shrink/merge_strategy_factory_sccs_options.h"

#include "language/plugins/plugin.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_sccs_order.h"

#include "downward_cli/utils/logging_options.h"

using namespace std;
using namespace downward;

using namespace probfd::merge_and_shrink;

using namespace language;
using namespace language::plugins;

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_sccs_options_to_feature(Feature& feature)
{
    feature.add_option<OrderOfSCCs>(
        "order_of_sccs",
        "how the SCCs should be ordered",
        "topological");
}

std::tuple<OrderOfSCCs> get_merge_strategy_sccs_args_from_options(
    const Context& context,
    const Options& options)
{
    return options.get<OrderOfSCCs>(context, "order_of_sccs");
}

} // namespace probfd::cli::merge_and_shrink
