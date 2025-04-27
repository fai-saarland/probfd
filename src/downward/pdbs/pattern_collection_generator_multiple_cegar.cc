#include "downward/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "downward/pdbs/cegar.h"
#include "downward/pdbs/pattern_database.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorMultipleCegar::
    PatternCollectionGeneratorMultipleCegar(
        bool use_wildcard_plans,
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
    , use_wildcard_plans(use_wildcard_plans)
{
}

string PatternCollectionGeneratorMultipleCegar::id() const
{
    return "CEGAR";
}

PatternInformation PatternCollectionGeneratorMultipleCegar::compute_pattern(
    int max_pdb_size,
    double max_time,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const SharedAbstractTask& task,
    const FactPair& goal,
    unordered_set<int>&& blacklisted_variables)
{
    utils::LogProxy silent_log = utils::get_silent_log();
    return generate_pattern_with_cegar(
        max_pdb_size,
        max_time,
        use_wildcard_plans,
        silent_log,
        rng,
        task,
        goal,
        std::move(blacklisted_variables));
}

} // namespace pdbs
