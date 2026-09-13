#include "probfd/merge_and_shrink/merge_strategy_factory_sccs_order.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeStrategyFactorySCCs::MergeStrategyFactorySCCs(
    utils::Verbosity verbosity,
    OrderOfSCCs order_of_sccs)
    : MergeStrategyFactory(verbosity)
    , order_of_sccs(order_of_sccs)
{
}

} // namespace probfd::merge_and_shrink
