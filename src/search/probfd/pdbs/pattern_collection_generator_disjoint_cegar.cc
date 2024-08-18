#include "probfd/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "probfd/pdbs/cegar/cegar.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/subcollection_finder_factory.h"
#include "probfd/pdbs/utils.h"

#include "probfd/task_proxy.h"

#include <utility>
#include <vector>

using namespace std;

namespace probfd::pdbs {

using namespace cegar;

PatternCollectionGeneratorDisjointCegar::
    PatternCollectionGeneratorDisjointCegar(
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
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    // Store the set of goals in random order.
    ProbabilisticTaskProxy task_proxy(*task);
    vector<int> goals = get_goals_in_random_order(task_proxy, *rng_);

    if (single_goal_) {
        goals.erase(goals.begin() + 1, goals.end());
    }

    CEGAR cegar(
        rng_,
        flaw_strategy_,
        use_wildcard_policies_,
        max_pdb_size_,
        max_collection_size_,
        std::move(goals));

    std::shared_ptr pdbs =
        cegar.generate_pdbs(task_proxy, *task_cost_function, max_time_, log_)
            .pdbs;

    auto patterns = std::make_shared<PatternCollection>();

    for (const auto& pdb : *pdbs) {
        patterns->push_back(pdb->get_pattern());
    }

    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory_->create_subcollection_finder(task_proxy);

    PatternCollectionInformation pattern_collection_information(
        task_proxy,
        task_cost_function,
        patterns,
        subcollection_finder);
    pattern_collection_information.set_pdbs(pdbs);
    return pattern_collection_information;
}

} // namespace probfd::pdbs
