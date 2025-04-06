#include "downward/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "downward/pdbs/cegar.h"
#include "downward/pdbs/utils.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng_options.h"

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorDisjointCegar::
    PatternCollectionGeneratorDisjointCegar(
        int max_pdb_size,
        int max_collection_size,
        double max_time,
        bool use_wildcard_plans,
        int random_seed,
        utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , max_pdb_size(max_pdb_size)
    , max_collection_size(max_collection_size)
    , max_time(max_time)
    , use_wildcard_plans(use_wildcard_plans)
    , rng(utils::get_rng(random_seed))
{
}

string PatternCollectionGeneratorDisjointCegar::name() const
{
    return "disjoint CEGAR pattern collection generator";
}

PatternCollectionInformation
PatternCollectionGeneratorDisjointCegar::compute_patterns(
    const shared_ptr<AbstractTask>& task)
{
    // Store the set of goals in random order.
    TaskProxy task_proxy(*task);
    vector<FactPair> goals = get_goals_in_random_order(task_proxy, *rng);

    return generate_pattern_collection_with_cegar(
        max_pdb_size,
        max_collection_size,
        max_time,
        use_wildcard_plans,
        log,
        rng,
        task,
        std::move(goals));
}

} // namespace pdbs
