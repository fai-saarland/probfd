#include "downward/cli/plugins/plugin.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_h_m.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_use_orders_option_to_feature;
using downward::cli::landmarks::get_use_orders_arguments_from_options;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;
using downward::cli::landmarks::get_landmark_factory_arguments_from_options;

namespace {

class LandmarkFactoryHMFeature
    : public TypedFeature<LandmarkFactory, LandmarkFactoryHM> {
public:
    LandmarkFactoryHMFeature()
        : TypedFeature("lm_hm")
    {
        // document_group("");
        document_title("h^m Landmarks");
        document_synopsis("The landmark generation method introduced by "
                          "Keyder, Richter & Helmert (ECAI 2010).");

        add_option<int>(
            "m",
            "subset size (if unsure, use the default of 2)",
            "2");
        add_option<bool>(
            "conjunctive_landmarks",
            "keep conjunctive landmarks",
            "true");
        add_use_orders_option_to_feature(*this);
        add_landmark_factory_options_to_feature(*this);

        document_language_support(
            "conditional_effects",
            "ignored, i.e. not supported");
    }

    virtual shared_ptr<LandmarkFactoryHM>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkFactoryHM>(
            opts.get<int>("m"),
            opts.get<bool>("conjunctive_landmarks"),
            get_use_orders_arguments_from_options(opts),
            get_landmark_factory_arguments_from_options(opts));
    }
};

FeaturePlugin<LandmarkFactoryHMFeature> _plugin;

} // namespace
