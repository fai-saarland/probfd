#include "downward/cli/landmarks/landmark_factory_rpg_sasp_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_sasp.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;
using downward::cli::landmarks::get_landmark_factory_arguments_from_options;

using downward::cli::landmarks::add_use_orders_option_to_feature;
using downward::cli::landmarks::get_use_orders_arguments_from_options;

namespace {
class LandmarkFactoryRpgSaspFeature : public SharedTypedFeature<LandmarkFactory> {
public:
    LandmarkFactoryRpgSaspFeature()
        : SharedTypedFeature("lm_rhw")
    {
        document_title("RHW Landmarks");
        document_synopsis(
            "The landmark generation method introduced by "
            "Richter, Helmert and Westphal (AAAI 2008).");

        add_option<bool>(
            "disjunctive_landmarks",
            "keep disjunctive landmarks",
            "true");
        add_use_orders_option_to_feature(*this);
        add_landmark_factory_options_to_feature(*this);

        document_language_support("conditional_effects", "supported");
    }

    virtual shared_ptr<LandmarkFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkFactoryRpgSasp>(
            opts.get<bool>("disjunctive_landmarks"),
            get_use_orders_arguments_from_options(opts),
            get_landmark_factory_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_rpg_sasp_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<LandmarkFactoryRpgSaspFeature>();
}

} // namespace downward::cli::landmarks
