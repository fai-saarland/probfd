#include "probfd/merge_and_shrink/shrink_strategy.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

void ShrinkStrategy::dump_options(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Shrink strategy options: " << endl;
        log << "Type: " << name() << endl;
        dump_strategy_specific_options(log);
    }
}

string ShrinkStrategy::get_name() const
{
    return name();
}

} // namespace probfd::merge_and_shrink