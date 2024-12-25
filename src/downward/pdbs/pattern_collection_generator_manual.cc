#include "downward/pdbs/pattern_collection_generator_manual.h"

#include "downward/pdbs/validation.h"

#include "downward/task_proxy.h"

#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace pdbs {
PatternCollectionGeneratorManual::PatternCollectionGeneratorManual(
    const vector<Pattern>& patterns,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , patterns(make_shared<PatternCollection>(patterns))
{
}

string PatternCollectionGeneratorManual::name() const
{
    return "manual pattern collection generator";
}

PatternCollectionInformation PatternCollectionGeneratorManual::compute_patterns(
    const shared_ptr<AbstractTask>& task)
{
    if (log.is_at_least_normal()) {
        log << "Manual pattern collection: " << *patterns << endl;
    }
    TaskProxy task_proxy(*task);
    return PatternCollectionInformation(task_proxy, patterns, log);
}

} // namespace pdbs
