#include "downward/pdbs/pattern_collection_generator_multiple_random.h"

#include "downward/pdbs/random_pattern.h"
#include "downward/pdbs/utils.h"

#include "downward/fact_pair.h"
#include "downward/state.h"

#include "downward/utils/logging.h"

#include <vector>

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorMultipleRandom::
    PatternCollectionGeneratorMultipleRandom(
        bool bidirectional,
        int max_pdb_size,
        int max_collection_size,
        double pattern_generation_max_time,
        double total_max_time,
        double stagnation_limit,
        double blacklist_trigger_percentage,
        bool enable_blacklist_on_stagnation,
        int random_seed,
        utils::Verbosity verbosity)
    : PatternCollectionGeneratorMultiple(
          max_pdb_size,
          max_collection_size,
          pattern_generation_max_time,
          total_max_time,
          stagnation_limit,
          blacklist_trigger_percentage,
          enable_blacklist_on_stagnation,
          random_seed,
          verbosity)
    , bidirectional(bidirectional)
{
}

string PatternCollectionGeneratorMultipleRandom::id() const
{
    return "random patterns";
}

void PatternCollectionGeneratorMultipleRandom::initialize(
    const shared_ptr<AbstractTask>& task)
{
    // Compute CG neighbors once. They are shuffled when used.
    cg_neighbors = compute_cg_neighbors(task, bidirectional);
}

PatternInformation PatternCollectionGeneratorMultipleRandom::compute_pattern(
    int max_pdb_size,
    double max_time,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const shared_ptr<AbstractTask>& task,
    const FactPair& goal,
    unordered_set<int>&&)
{
    // TODO: add support for blacklisting in single RCG?
    utils::LogProxy silent_log = utils::get_silent_log();
    Pattern pattern = generate_random_pattern(
        max_pdb_size,
        max_time,
        silent_log,
        rng,
        *task,
        goal.var,
        cg_neighbors);

    PatternInformation result(*task, std::move(pattern), log);
    return result;
}

} // namespace pdbs
