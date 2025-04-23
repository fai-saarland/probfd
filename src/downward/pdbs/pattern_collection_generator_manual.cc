#include "downward/pdbs/pattern_collection_generator_manual.h"

#include "downward/pdbs/validation.h"

#include "downward/state.h"

#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace downward::pdbs {
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
    return PatternCollectionInformation(*task, patterns, log);
}

} // namespace pdbs
