#include "downward/pdbs/pattern_collection_generator_manual.h"

#include "downward/pdbs/validation.h"

#include "downward/state.h"

#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorManual::PatternCollectionGeneratorManual(
    vector<Pattern> patterns,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , patterns(make_shared<PatternCollection>(std::move(patterns)))
{
}

string PatternCollectionGeneratorManual::name() const
{
    return "manual pattern collection generator";
}

PatternCollectionInformation PatternCollectionGeneratorManual::compute_patterns(
    const SharedAbstractTask& task)
{
    if (log.is_at_least_normal()) {
        log.println("Manual pattern collection: {}", *patterns);
    }
    return PatternCollectionInformation(to_refs(task), patterns, log);
}

} // namespace pdbs
