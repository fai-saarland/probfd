#include "probfd/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "probfd/pdbs/cegar/cegar.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/subcollection_finder_factory.h"
#include "probfd/pdbs/utils.h"

#include "probfd/task_proxy.h"

#include "downward/utils/rng_options.h"

#include "downward/plugins/plugin.h"

#include <utility>
#include <vector>

using namespace std;

namespace probfd::pdbs {

using namespace cegar;

PatternCollectionGeneratorDisjointCegar::
    PatternCollectionGeneratorDisjointCegar(const plugins::Options& opts)
    : PatternCollectionGenerator(opts)
    , use_wildcard_policies_(opts.get<bool>("use_wildcard_policies"))
    , single_goal_(opts.get<bool>("single_goal"))
    , max_pdb_size_(opts.get<int>("max_pdb_size"))
    , max_collection_size_(opts.get<int>("max_collection_size"))
    , max_time_(opts.get<double>("max_time"))
    , rng_(utils::parse_rng_from_options(opts))
    , subcollection_finder_factory_(
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"))
    , flaw_strategy_(
          opts.get<std::shared_ptr<FlawFindingStrategy>>("flaw_strategy"))
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

void add_pattern_collection_generator_cegar_options_to_feature(
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
    feature.add_option<std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "The subcollection finder factory.",
        "finder_trivial_factory()");
    feature.add_option<std::shared_ptr<FlawFindingStrategy>>(
        "flaw_strategy",
        "strategy used to find flaws in a policy",
        "pucs_flaw_finder()");

    add_pattern_collection_generator_options_to_feature(feature);
    add_cegar_wildcard_option_to_feature(feature);
}

class PatternCollectionGeneratorDisjointCEGARFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorDisjointCegar> {
public:
    PatternCollectionGeneratorDisjointCEGARFeature()
        : TypedFeature("ppdbs_disjoint_cegar")
    {
        add_pattern_collection_generator_cegar_options_to_feature(*this);
        utils::add_rng_options(*this);
    }
};

static plugins::FeaturePlugin<PatternCollectionGeneratorDisjointCEGARFeature>
    _plugin;

} // namespace probfd::pdbs
