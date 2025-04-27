#include "probfd/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "downward/initial_state_values.h"
#include "probfd/pdbs/cegar/cegar.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/subcollection_finder_factory.h"
#include "probfd/pdbs/utils.h"

#include <utility>
#include <vector>

using namespace std;
using namespace downward;

namespace probfd::pdbs {

using namespace cegar;

PatternCollectionGeneratorDisjointCegar::
    PatternCollectionGeneratorDisjointCegar(
        value_t convergence_epsilon,
        bool use_wildcard_policies,
        bool single_goal,
        int max_pdb_size,
        int max_collection_size,
        double max_time,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        const std::shared_ptr<SubCollectionFinderFactory>&
            subcollection_finder_factory,
        const std::shared_ptr<FlawFindingStrategy>& flaw_strategy,
        utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , convergence_epsilon_(convergence_epsilon)
    , use_wildcard_policies_(use_wildcard_policies)
    , single_goal_(single_goal)
    , max_pdb_size_(max_pdb_size)
    , max_collection_size_(max_collection_size)
    , max_time_(max_time)
    , rng_(std::move(rng))
    , subcollection_finder_factory_(subcollection_finder_factory)
    , flaw_strategy_(flaw_strategy)
{
}

PatternCollectionInformation PatternCollectionGeneratorDisjointCegar::generate(
    const SharedProbabilisticTask& task)
{
    const auto& goals = get_goal(task);

    // Store the set of goals in random order.
    vector<int> goal_facts = get_goals_in_random_order(goals, *rng_);

    if (single_goal_) {
        goal_facts.erase(goal_facts.begin() + 1, goal_facts.end());
    }

    CEGAR cegar(
        convergence_epsilon_,
        rng_,
        flaw_strategy_,
        use_wildcard_policies_,
        max_pdb_size_,
        max_collection_size_,
        std::move(goal_facts));

    ProjectionCollection projections;
    PPDBCollection pdbs;

    const State initial_state = get_init(task).get_initial_state();

    cegar
        .generate_pdbs(task, initial_state, projections, pdbs, max_time_, log_);

    PatternCollection patterns;

    for (const auto& pdb : pdbs) { patterns.push_back(pdb->get_pattern()); }

    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory_->create_subcollection_finder(task);

    PatternCollectionInformation pattern_collection_information(
        task,
        patterns,
        subcollection_finder);
    pattern_collection_information.set_pdbs(pdbs);
    return pattern_collection_information;
}

} // namespace probfd::pdbs
