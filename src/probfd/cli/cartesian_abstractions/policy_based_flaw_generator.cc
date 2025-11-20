#include "probfd/cli/cartesian_abstractions/policy_based_flaw_generator.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

namespace {
class ILAOFlawGeneratorFactoryFeature
    : public SharedTypedFeature<FlawGeneratorFactory, probfd::value_t, int> {
public:
    ILAOFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_ilao", &ILAOFlawGeneratorFactoryFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");

        make_optional_argument_with_default(
            1,
            "max_search_states",
            "infinity()",
            "maximum number of concrete states allowed to be generated during "
            "flaw "
            "search before giving up");
    }

    static std::shared_ptr<FlawGeneratorFactory> func(
        probfd::value_t convergence_epsilon,
        int max_search_states)
    {
        return std::make_shared<ILAOFlawGeneratorFactory>(
            convergence_epsilon,
            max_search_states);
    }
};
} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_policy_based_flaw_generator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<ILAOFlawGeneratorFactoryFeature>();
}

} // namespace probfd::cli::cartesian_abstractions