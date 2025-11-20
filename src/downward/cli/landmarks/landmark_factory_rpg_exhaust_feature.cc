#include "downward/cli/landmarks/landmark_factory_rpg_exhaust_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_exhaust.h"

using namespace std;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

using namespace downward::landmarks;

namespace {
class LandmarkFactoryRpgExhaustFeature
    : public SharedTypedFeature<
          LandmarkFactory,
          bool,
          downward::utils::Verbosity> {
public:
    LandmarkFactoryRpgExhaustFeature()
        : TypedFeature("lm_exhaust", &LandmarkFactoryRpgExhaustFeature::func)
    {
        document_title("Exhaustive Landmarks");
        document_synopsis(
            "Exhaustively checks for each fact if it is a landmark."
            "This check is done using relaxed planning.");

        document_language_support(
            "conditional_effects",
            "ignored, i.e. not supported");

        make_optional_argument_with_default(
            0,
            "use_unary_relaxation",
            "false",
            "compute landmarks of the unary relaxation, i.e., landmarks "
            "for the delete relaxation of a task transformation such that each "
            "operator is split into one operator for each of its effects. This "
            "kind of landmark was previously known as \"causal landmarks\". "
            "Setting the option to true can reduce the overall number of "
            "landmarks, which can make the search more memory-efficient but "
            "potentially less informative.");
        add_landmark_factory_options_to_feature(*this, 1);
    }

    static shared_ptr<LandmarkFactory> func(
        bool use_unary_relaxation,
        downward::utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<LandmarkFactoryRpgExhaust>(
            use_unary_relaxation,
            verbosity);
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_rpg_exhaust_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LandmarkFactoryRpgExhaustFeature>();
}

} // namespace downward::cli::landmarks
