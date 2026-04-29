#include "probfd/merge_and_shrink/merge_strategy_factory.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

void MergeStrategyFactory::dump_options(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log.println("Merge strategy options:");
        log.println("Type: {}", name());
        dump_strategy_specific_options(log);
    }
}

} // namespace probfd::merge_and_shrink
