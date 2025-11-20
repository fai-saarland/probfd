#include "downward/cli/landmarks/landmark_factory_rpg_sasp_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_sasp.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

using downward::cli::landmarks::add_use_orders_option_to_feature;

namespace {
class LandmarkFactoryRpgSaspFeature
    : public SharedTypedFeature<
          LandmarkFactory,
          bool,
          bool,
          downward::utils::Verbosity> {
public:
    LandmarkFactoryRpgSaspFeature()
        : TypedFeature("lm_rhw", &LandmarkFactoryRpgSaspFeature::func)
    {
        document_title("RHW Landmarks");
        document_synopsis(
            "The landmark generation method introduced by "
            "Richter, Helmert and Westphal (AAAI 2008).");
        document_language_support("conditional_effects", "supported");

        make_optional_argument_with_default(
            0,
            "disjunctive_landmarks",
            "true",
            "keep disjunctive landmarks");
        const auto n = add_use_orders_option_to_feature(*this, 1);
        add_landmark_factory_options_to_feature(*this, n + 1);
    }

    static shared_ptr<LandmarkFactory> func(
        bool disjunctive_landmarks,
        bool use_orders,
        downward::utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<LandmarkFactoryRpgSasp>(
            disjunctive_landmarks,
            use_orders,
            verbosity);
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_rpg_sasp_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LandmarkFactoryRpgSaspFeature>();
}

} // namespace downward::cli::landmarks
