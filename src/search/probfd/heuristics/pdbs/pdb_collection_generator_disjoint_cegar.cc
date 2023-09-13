#include "probfd/heuristics/pdbs/pdb_collection_generator_disjoint_cegar.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"
#include "probfd/heuristics/pdbs/pdb_collection_information.h"
#include "probfd/heuristics/pdbs/pdb_combinator_factory.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/plugins/plugin.h"

#include <stack>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace cegar;

PDBCollectionGeneratorDisjointCegar::PDBCollectionGeneratorDisjointCegar(
    const plugins::Options& opts)
    : PDBCollectionGenerator(opts)
    , use_wildcard_policies(opts.get<bool>("use_wildcard_policies"))
    , single_goal(opts.get<bool>("single_goal"))
    , max_pdb_size(opts.get<int>("max_pdb_size"))
    , max_collection_size(opts.get<int>("max_collection_size"))
    , max_time(opts.get<double>("max_time"))
    , rng(utils::parse_rng_from_options(opts))
    , pdb_combinator_factory(opts.get<std::shared_ptr<PDBCombinatorFactory>>(
          "pdb_combinator_factory"))
    , exploration_strategy(opts.get<std::shared_ptr<PolicyExplorationStrategy>>(
          "exploration_strategy"))
{
}

PDBCollectionInformation PDBCollectionGeneratorDisjointCegar::generate(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    // Store the set of goals in random order.
    ProbabilisticTaskProxy task_proxy(*task);
    vector<int> goals = get_goals_in_random_order(task_proxy, *rng);

    if (single_goal) {
        goals.erase(goals.begin() + 1, goals.end());
    }

    CEGAR cegar(
        rng,
        exploration_strategy,
        use_wildcard_policies,
        max_pdb_size,
        max_collection_size,
        std::move(goals));

    utils::CountdownTimer timer(max_time);

    PPDBCollection pdbs =
        cegar.generate_pdbs(task_proxy, *task_cost_function, log, timer).pdbs;

    std::shared_ptr<PDBCombinator> pdb_combinator =
        pdb_combinator_factory->create_pdb_combinator(task_proxy, pdbs);

    return PDBCollectionInformation(std::move(pdbs), std::move(pdb_combinator));
}

void add_pdb_collection_generator_cegar_options_to_feature(
    plugins::Feature& feature)
{
    feature.add_option<bool>(
        "single_goal",
        "whether to compute only a single abstraction from a random goal",
        "false");
    feature.add_option<int>(
        "max_pdb_size",
        "maximum allowed number of states in a pdb (not applied to initial "
        "goal variable pattern(s))",
        "1000000",
        plugins::Bounds("1", "infinity"));
    feature.add_option<int>(
        "max_collection_size",
        "limit for the total number of PDB entries across all PDBs (not "
        "applied to initial goal variable pattern(s))",
        "infinity",
        plugins::Bounds("1", "infinity"));
    feature.add_option<double>(
        "max_time",
        "maximum time in seconds for CEGAR pattern generation. "
        "This includes the creation of the initial PDB collection"
        " as well as the creation of the correlation matrix.",
        "infinity",
        plugins::Bounds("0.0", "infinity"));
    feature.add_option<std::shared_ptr<PDBCombinatorFactory>>(
        "pdb_combinator_factory",
        "The pdb combinator factory.",
        "maximum_combinator_factory()");
    feature.add_option<std::shared_ptr<PolicyExplorationStrategy>>(
        "exploration_strategy",
        "strategy used to explore abstract policies",
        "mlp()");

    add_pdb_collection_generator_options_to_feature(feature);
    add_cegar_wildcard_option_to_feature(feature);
}

class PDBCollectionGeneratorDisjointCEGARFeature
    : public plugins::TypedFeature<
          PDBCollectionGenerator,
          PDBCollectionGeneratorDisjointCegar> {
public:
    PDBCollectionGeneratorDisjointCEGARFeature()
        : TypedFeature("ppdbs_disjoint_cegar")
    {
        add_pdb_collection_generator_cegar_options_to_feature(*this);
        utils::add_rng_options(*this);
    }
};

static plugins::FeaturePlugin<PDBCollectionGeneratorDisjointCEGARFeature>
    _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd