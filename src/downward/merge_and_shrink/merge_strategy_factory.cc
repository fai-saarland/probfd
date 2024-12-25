#include "downward/merge_and_shrink/merge_strategy_factory.h"

#include <iostream>

using namespace std;

namespace merge_and_shrink {
MergeStrategyFactory::MergeStrategyFactory(utils::Verbosity verbosity)
    : log(utils::get_log_for_verbosity(verbosity))
{
}

void MergeStrategyFactory::dump_options() const
{
    if (log.is_at_least_normal()) {
        log << "Merge strategy options:" << endl;
        log << "Type: " << name() << endl;
        dump_strategy_specific_options();
    }
}

} // namespace merge_and_shrink
