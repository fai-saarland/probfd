#include "probfd/merge_and_shrink/merge_strategy_factory.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeStrategyFactory::MergeStrategyFactory(utils::Verbosity verbosity)
    : log(get_log_for_verbosity(verbosity))
{
}

void MergeStrategyFactory::dump_options() const
{
    if (log.is_at_least_normal()) {
        log.println("Merge strategy options:");
        log.println("Type: {}", name());
        dump_strategy_specific_options();
    }
}

} // namespace probfd::merge_and_shrink
