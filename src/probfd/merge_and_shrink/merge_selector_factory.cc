#include "probfd/merge_and_shrink/merge_selector_factory.h"

#include "downward/utils/logging.h"

using namespace downward;

namespace probfd::merge_and_shrink {

void MergeSelectorFactory::dump_options(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log.println("Merge selector options:");
        log.println("Name: {}", name());
        dump_selector_specific_options(log);
    }
}

} // namespace probfd::merge_and_shrink