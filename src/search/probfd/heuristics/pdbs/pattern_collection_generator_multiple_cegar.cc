#include "probfd/heuristics/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "probfd/heuristics/pdbs/cegar/single_cegar.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_info.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/cost_function.h"

#include "downward/utils/countdown_timer.h"
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

ProjectionInfo PatternCollectionGeneratorMultipleCegar::compute_pattern(
    int max_pdb_size,
    double max_time,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const FactPair& goal,
    unordered_set<int>&& blacklisted_variables)
{
    SingleCEGAR cegar(
        log,
        rng,
        flaw_strategy,
        use_wildcard_policies,
        max_pdb_size,
        max_time,
        goal.var,
        std::move(blacklisted_variables));

    utils::CountdownTimer timer(max_time);

    // Start with the single goal var projection
    ProjectionInfo projection_info(
        task_proxy,
        task_cost_function,
        Pattern{goal.var},
        task_proxy.get_initial_state(),
        BlindEvaluator<AbstractStateIndex>(),
        false,
        timer.get_remaining_time());

    cegar.run_refinement_loop(
        task_proxy,
        task_cost_function,
        projection_info,
        timer);
    return projection_info;
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