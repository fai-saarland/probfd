#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_sasp.h"

using namespace std;
using namespace landmarks;
using namespace utils;

using namespace downward_plugins::plugins;

using downward_plugins::landmarks::add_landmark_factory_options_to_feature;
using downward_plugins::landmarks::get_landmark_factory_arguments_from_options;

using downward_plugins::landmarks::add_use_orders_option_to_feature;
using downward_plugins::landmarks::get_use_orders_arguments_from_options;

using downward_plugins::landmarks::add_only_causal_landmarks_option_to_feature;
using downward_plugins::landmarks::
    get_only_causal_landmarks_arguments_from_options;

namespace {

class LandmarkFactoryRpgSaspFeature
    : public TypedFeature<LandmarkFactory, LandmarkFactoryRpgSasp> {
public:
    LandmarkFactoryRpgSaspFeature()
        : TypedFeature("lm_rhw")
    {
        document_title("RHW Landmarks");
        document_synopsis("The landmark generation method introduced by "
                          "Richter, Helmert and Westphal (AAAI 2008).");

        add_option<bool>(
            "disjunctive_landmarks",
            "keep disjunctive landmarks",
            "true");
        add_landmark_factory_options_to_feature(*this);
        add_use_orders_option_to_feature(*this);
        add_only_causal_landmarks_option_to_feature(*this);

        document_language_support("conditional_effects", "supported");
    }

    virtual shared_ptr<LandmarkFactoryRpgSasp>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkFactoryRpgSasp>(
            opts.get<bool>("disjunctive_landmarks"),
            get_use_orders_arguments_from_options(opts),
            get_only_causal_landmarks_arguments_from_options(opts),
            get_landmark_factory_arguments_from_options(opts));
    }
};

FeaturePlugin<LandmarkFactoryRpgSaspFeature> _plugin;

} // namespace
