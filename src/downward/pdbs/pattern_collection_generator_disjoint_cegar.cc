#include "downward/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "downward/pdbs/cegar.h"
#include "downward/pdbs/utils.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorDisjointCegar::
    PatternCollectionGeneratorDisjointCegar(
        int max_pdb_size,
        int max_collection_size,
        utils::FSeconds max_time,
        bool use_wildcard_plans,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , max_pdb_size(max_pdb_size)
    , max_collection_size(max_collection_size)
    , max_time(max_time)
    , use_wildcard_plans(use_wildcard_plans)
    , rng(std::move(rng))
{
    if (max_pdb_size < 1) {
        throw std::domain_error("max_pdb_size must be >= 1.");
    }

    if (max_collection_size < 1) {
        throw std::domain_error("max_collection_size must be >= 1.");
    }

    if (max_time.count() < 0) {
        throw std::domain_error("max_time must be >= 0.");
    }
}

string PatternCollectionGeneratorDisjointCegar::name() const
{
    return "disjoint CEGAR pattern collection generator";
}

PatternCollectionInformation
PatternCollectionGeneratorDisjointCegar::compute_patterns(
    const SharedAbstractTask& task)
{
    // Store the set of goals in random order.
    vector<FactPair> goals = get_goals_in_random_order(get_goal(task), *rng);

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

} // namespace downward::pdbs
