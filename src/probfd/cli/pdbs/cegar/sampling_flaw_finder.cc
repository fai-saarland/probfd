#include "probfd/cli/pdbs/cegar/sampling_flaw_finder.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/pdbs/cegar/sampling_flaw_finder.h"

#include "downward/utils/rng_options.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
class SamplingFlawFinderFeature
    : public SharedTypedFeature<FlawFindingStrategy, int, int> {
public:
    SamplingFlawFinderFeature()
        : TypedFeature("sampling_flaw_finder", &SamplingFlawFinderFeature::func)
    {
        const auto n = add_rng_options_to_feature(*this, 0);
        make_optional_argument_with_default(
            n,
            "max_search_states",
            "20M",
            "Maximal number of generated states after which the flaw search is "
            "aborted.");
    }

    static std::shared_ptr<FlawFindingStrategy>
    func(int random_seed, int max_search_states)
    {
        return make_shared_from_arg_tuples<SamplingFlawFinder>(
            get_rng(random_seed),
            max_search_states);
    }
};
} // namespace

namespace probfd::cli::pdbs::cegar {

void add_sampling_flaw_finder_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<SamplingFlawFinderFeature>();
}

} // namespace probfd::cli::pdbs::cegar
