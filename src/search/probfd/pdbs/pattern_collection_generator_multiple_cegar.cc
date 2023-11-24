#include "probfd/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "probfd/pdbs/cegar/cegar.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/cost_function.h"

#include "downward/utils/logging.h"

#include "downward/plugins/plugin.h"

using namespace std;

namespace probfd {
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
    CEGAR cegar(
        log,
        rng,
        flaw_strategy,
        use_wildcard_policies,
        max_pdb_size,
        max_pdb_size,
        max_time,
        {goal.var},
        std::move(blacklisted_variables));
    auto [state_spaces, pdbs] =
        cegar.generate_pdbs(task_proxy, task_cost_function);
    assert(state_spaces->size() == pdbs->size());
    if (pdbs->size() > 1) {
        cerr << "CEGAR limited to one goal computed more than one pattern"
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    return {std::move(state_spaces->front()), pdbs->front()};
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
} // namespace probfd