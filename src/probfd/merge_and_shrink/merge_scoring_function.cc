#include "probfd/merge_and_shrink/merge_scoring_function.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

void MergeScoringFunction::dump_options(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Merge scoring function:" << endl;
        log << "Name: " << name() << endl;
        dump_function_specific_options(log);
    }
}

} // namespace probfd::merge_and_shrink