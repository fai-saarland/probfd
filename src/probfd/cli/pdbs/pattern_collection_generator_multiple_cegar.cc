#include "probfd/cli/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/cli/pdbs/cegar/cegar_options.h"
#include "probfd/cli/pdbs/pattern_collection_generator_multiple_options.h"

#include "probfd/pdbs/pattern_collection_generator_multiple_cegar.h"

using namespace downward;
using namespace utils;

using namespace probfd::cli::pdbs;

using namespace probfd::pdbs;
using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

namespace {
class PatternCollectionGeneratorMultipleCegarFeature
    : public SharedTypedFeature<
          PatternCollectionGenerator,
          probfd::value_t,
          std::shared_ptr<FlawFindingStrategy>,
          bool,
          int,
          int,
          FSeconds,
          FSeconds,
          FSeconds,
          double,
          bool,
          bool,
          std::shared_ptr<RandomNumberGenerator>,
          Verbosity> {
public:
    PatternCollectionGeneratorMultipleCegarFeature()
        : TypedFeature(
              "ppdbs_multiple_cegar",
              &PatternCollectionGeneratorMultipleCegarFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "convergence_epsilon",
            "10e-8",
            "The tolerance for convergence checks.");

        make_optional_argument_with_default(
            1,
            "flaw_strategy",
            "pucs_flaw_finder()",
            "strategy used to find flaws in a policy");
        const auto n = add_cegar_wildcard_option_to_feature(*this, 2);
        add_multiple_options_to_feature(*this, n + 2);
    }

    static std::shared_ptr<PatternCollectionGenerator> func(
        probfd::value_t convergence_epsilon,
        std::shared_ptr<FlawFindingStrategy> flaw_strategy,
        bool use_wildcard_policies,
        int max_pdb_size,
        int max_collection_size,
        FSeconds pattern_generation_max_time,
        FSeconds total_max_time,
        FSeconds stagnation_limit,
        double blacklist_trigger_percentage,
        bool enable_blacklist_on_stagnation,
        bool use_saturated_costs,
        std::shared_ptr<RandomNumberGenerator> rng,
        Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorMultipleCegar>(
            convergence_epsilon,
            std::move(flaw_strategy),
            use_wildcard_policies,
            max_pdb_size,
            max_collection_size,
            pattern_generation_max_time,
            total_max_time,
            stagnation_limit,
            blacklist_trigger_percentage,
            enable_blacklist_on_stagnation,
            use_saturated_costs,
            std::move(rng),
            verbosity);
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_multiple_cegar_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternCollectionGeneratorMultipleCegarFeature>();
}

} // namespace probfd::cli::pdbs
