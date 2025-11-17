#include "downward/cli/landmarks/landmark_factory_zhu_givan_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_zhu_givan.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_use_orders_option_to_feature;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

namespace {
class LandmarkFactoryZhuGivanFeature
    : public SharedTypedFeature<
          LandmarkFactory,
          bool,
          downward::utils::Verbosity> {
public:
    LandmarkFactoryZhuGivanFeature()
        : TypedFeature("lm_zg", &LandmarkFactoryZhuGivanFeature::func)
    {
        document_title("Zhu/Givan Landmarks");
        document_synopsis(
            "The landmark generation method introduced by "
            "Zhu & Givan (ICAPS 2003 Doctoral Consortium).");

        const auto n = add_use_orders_option_to_feature(*this, 0);
        add_landmark_factory_options_to_feature(*this, n);

        // TODO: Make sure that conditional effects are indeed supported.
        document_language_support(
            "conditional_effects",
            "We think they are supported, but this is not 100% sure.");
    }

    static shared_ptr<LandmarkFactory>
    func(const Context&, bool use_orders, downward::utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<LandmarkFactoryZhuGivan>(
            use_orders,
            verbosity);
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_zhu_givan_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LandmarkFactoryZhuGivanFeature>();
}

} // namespace downward::cli::landmarks
