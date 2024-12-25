#include "downward/pdbs/pattern_generator.h"

#include "downward/pdbs/utils.h"

using namespace std;

namespace pdbs {
PatternCollectionGenerator::PatternCollectionGenerator(
    utils::Verbosity verbosity)
    : log(utils::get_log_for_verbosity(verbosity))
{
}

PatternCollectionInformation
PatternCollectionGenerator::generate(const shared_ptr<AbstractTask>& task)
{
    if (log.is_at_least_normal()) {
        log << "Generating patterns using: " << name() << endl;
    }
    utils::Timer timer;
    PatternCollectionInformation pci = compute_patterns(task);
    dump_pattern_collection_generation_statistics(name(), timer(), pci, log);
    return pci;
}

PatternGenerator::PatternGenerator(utils::Verbosity verbosity)
    : log(utils::get_log_for_verbosity(verbosity))
{
}

PatternInformation
PatternGenerator::generate(const shared_ptr<AbstractTask>& task)
{
    if (log.is_at_least_normal()) {
        log << "Generating pattern using: " << name() << endl;
    }
    utils::Timer timer;
    PatternInformation pattern_info = compute_pattern(task);
    dump_pattern_generation_statistics(name(), timer.stop(), pattern_info, log);
    return pattern_info;
}

} // namespace pdbs
