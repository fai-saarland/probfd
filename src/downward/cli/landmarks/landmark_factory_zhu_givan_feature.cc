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

Feature& add_landmark_factory_zhu_givan_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "lm_zg",
        &downward::cli::plugins::make_shared<
            LandmarkFactory,
            LandmarkFactoryZhuGivan,
            bool,
            Verbosity>);
    f.document_title("Zhu/Givan Landmarks");
    f.document_synopsis(
        "The landmark generation method introduced by "
        "Zhu & Givan (ICAPS 2003 Doctoral Consortium).");

    const auto n = add_use_orders_option_to_feature(f, 0);
    add_landmark_factory_options_to_feature(f, n);

    // TODO: Make sure that conditional effects are indeed supported.
    f.document_language_support(
        "conditional_effects",
        "We think they are supported, but this is not 100% sure.");

    return f;
}

} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_zhu_givan_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_landmark_factory_zhu_givan_to_namespace(n);
}

} // namespace downward::cli::landmarks
