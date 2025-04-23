#include "downward/pdbs/pattern_generator_manual.h"

#include "downward/pdbs/pattern_information.h"

#include "downward/state.h"

#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace downward::pdbs {
PatternGeneratorManual::PatternGeneratorManual(
    const vector<int>& pattern,
    utils::Verbosity verbosity)
    : PatternGenerator(verbosity)
    , pattern(pattern)
{
}

string PatternGeneratorManual::name() const
{
    return "manual pattern generator";
}

PatternInformation
PatternGeneratorManual::compute_pattern(const shared_ptr<AbstractTask>& task)
{
    PatternInformation pattern_info(*task, std::move(pattern), log);
    if (log.is_at_least_normal()) {
        log << "Manual pattern: " << pattern_info.get_pattern() << endl;
    }
    return pattern_info;
}

} // namespace pdbs
