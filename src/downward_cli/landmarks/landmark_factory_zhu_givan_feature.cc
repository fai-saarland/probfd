#include "downward_cli/landmarks/landmark_factory_zhu_givan_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_zhu_givan.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace language;
using namespace language::plugins;

using downward::cli::landmarks::add_use_orders_option_to_feature;
using downward::cli::landmarks::get_use_orders_arguments_from_options;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;
using downward::cli::landmarks::get_landmark_factory_arguments_from_options;

namespace {
class LandmarkFactoryZhuGivanFeature : public TypedFeature<LandmarkFactory> {
public:
    LandmarkFactoryZhuGivanFeature()
        : TypedFeature("lm_zg")
    {
        document_title("Zhu/Givan Landmarks");
        document_synopsis(
            "The landmark generation method introduced by "
            "Zhu & Givan (ICAPS 2003 Doctoral Consortium).");

        add_use_orders_option_to_feature(*this);
        add_landmark_factory_options_to_feature(*this);

        // TODO: Make sure that conditional effects are indeed supported.
        document_language_support(
            "conditional_effects",
            "We think they are supported, but this is not 100% sure.");
    }

    shared_ptr<LandmarkFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<LandmarkFactoryZhuGivan>(
            get_use_orders_arguments_from_options(context, opts),
            get_landmark_factory_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_zhu_givan_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<LandmarkFactoryZhuGivanFeature>();
}

} // namespace downward::cli::landmarks
