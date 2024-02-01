#include "probfd/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "probfd/pdbs/cegar/single_cegar.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_proxy.h"

#include "downward/plugins/plugin.h"

#include "downward/abstract_task.h"

#include <cassert>
#include <iostream>
#include <vector>

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
    SingleCEGAR single_cegar(
        rng,
        flaw_strategy_,
        use_wildcard_policies_,
        max_pdb_size,
        max_time,
        goal.var,
        std::move(blacklisted_variables));
    auto [state_space, pdb] =
        single_cegar.generate_pdbs(task_proxy, task_cost_function, log_);

    return {std::move(state_space), std::move(pdb)};
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
