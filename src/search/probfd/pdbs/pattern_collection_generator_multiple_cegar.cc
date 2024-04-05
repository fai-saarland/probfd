#include "probfd/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "probfd/pdbs/cegar/single_cegar.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_proxy.h"

#include "downward/plugins/plugin.h"

#include "downward/abstract_task.h"
#include "downward/utils/countdown_timer.h"

#include <iostream>

using namespace std;

namespace probfd::pdbs {

using namespace cegar;

PatternCollectionGeneratorMultipleCegar::
    PatternCollectionGeneratorMultipleCegar(const plugins::Options& opts)
    : PatternCollectionGeneratorMultiple(opts, "CEGAR")
    , use_wildcard_policies_(opts.get<bool>("use_wildcard_policies"))
    , flaw_strategy_(
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
    utils::CountdownTimer timer(max_time);

    // Start with a solution of the trivial abstraction
    StateRankingFunction ranking_function(
        task_proxy.get_variables(),
        {goal.var});

    auto projection = std::make_unique<ProjectionStateSpace>(
        task_proxy,
        task_cost_function,
        ranking_function,
        false,
        timer.get_remaining_time());

    StateRank init_state_rank =
        ranking_function.get_abstract_rank(task_proxy.get_initial_state());

    auto pdb = std::make_unique<ProbabilityAwarePatternDatabase>(
        *projection,
        std::move(ranking_function),
        init_state_rank,
        heuristics::ConstantEvaluator<StateRank>(0_vt),
        timer.get_remaining_time());

    SingleCEGARResult result(std::move(projection), std::move(pdb));

    run_cegar_loop(
        result,
        task_proxy,
        task_cost_function,
        *flaw_strategy_,
        std::move(blacklisted_variables),
        max_pdb_size,
        *rng,
        use_wildcard_policies_,
        max_time,
        log_);

    return std::make_pair(std::move(result.projection), std::move(result.pdb));
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

} // namespace probfd::pdbs
