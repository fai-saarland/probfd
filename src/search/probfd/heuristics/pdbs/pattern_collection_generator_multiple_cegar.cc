#include "probfd/heuristics/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_generator.h"
#include "probfd/heuristics/pdbs/cegar/projection_factory.h"
#include "probfd/heuristics/pdbs/cegar/single_cegar.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"

#include "probfd/cost_function.h"

#include "downward/utils/logging.h"

#include "downward/plugins/plugin.h"

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace cegar;

PatternCollectionGeneratorMultipleCegar::
    PatternCollectionGeneratorMultipleCegar(const plugins::Options& opts)
    : PatternCollectionGeneratorMultiple(opts, "CEGAR")
    , use_wildcard_policies(opts.get<bool>("use_wildcard_policies"))
    , flaw_strategy(
          opts.get<std::shared_ptr<FlawFindingStrategy>>("flaw_strategy"))
{
}

std::pair<
    std::shared_ptr<ProjectionStateSpace>,
    std::shared_ptr<ProbabilityAwarePatternDatabase>>
PatternCollectionGeneratorMultipleCegar::compute_pattern(
    int max_pdb_size,
    double max_time,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const FactPair& goal,
    unordered_set<int>&& blacklisted_variables)
{
    SingleCEGAR cegar(
        max_pdb_size,
        max_time,
        std::make_shared<FlawGenerator>(
            flaw_strategy,
            rng,
            use_wildcard_policies),
        std::make_shared<ProjectionFactory>(goal.var),
        log,
        std::move(blacklisted_variables));

    auto [abstraction_mapping, state_space, heuristic] =
        cegar.generate_pdb(task_proxy, task_cost_function);

    return {
        std::move(state_space),
        std::make_shared<ProbabilityAwarePatternDatabase>(
            std::move(*abstraction_mapping),
            std::move(heuristic->get_value_table()))};
}

class PatternCollectionGeneratorMultipleCegarFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorMultipleCegar> {
public:
    PatternCollectionGeneratorMultipleCegarFeature()
        : TypedFeature("ppdbs_multiple_cegar")
    {
        add_multiple_options_to_feature(*this);
        add_cegar_wildcard_option_to_feature(*this);

        add_option<std::shared_ptr<cegar::FlawFindingStrategy>>(
            "flaw_strategy",
            "strategy used to find flaws in a policy",
            "pucs_flaw_finder()");
    }
};

static plugins::FeaturePlugin<PatternCollectionGeneratorMultipleCegarFeature>
    _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd