#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_exhaust.h"

using namespace std;
using namespace utils;

using namespace downward_plugins::plugins;

using downward_plugins::landmarks::add_only_causal_landmarks_option_to_feature;
using downward_plugins::landmarks::
    get_only_causal_landmarks_arguments_from_options;

using downward_plugins::landmarks::add_landmark_factory_options_to_feature;
using downward_plugins::landmarks::get_landmark_factory_arguments_from_options;

namespace landmarks {

class LandmarkFactoryRpgExhaustFeature
    : public TypedFeature<LandmarkFactory, LandmarkFactoryRpgExhaust> {
public:
    LandmarkFactoryRpgExhaustFeature()
        : TypedFeature("lm_exhaust")
    {
        document_title("Exhaustive Landmarks");
        document_synopsis(
            "Exhaustively checks for each fact if it is a landmark."
            "This check is done using relaxed planning.");

        add_only_causal_landmarks_option_to_feature(*this);
        add_landmark_factory_options_to_feature(*this);

        document_language_support(
            "conditional_effects",
            "ignored, i.e. not supported");
    }

    virtual shared_ptr<LandmarkFactoryRpgExhaust>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkFactoryRpgExhaust>(
            get_only_causal_landmarks_arguments_from_options(opts),
            get_landmark_factory_arguments_from_options(opts));
    }
};

FeaturePlugin<LandmarkFactoryRpgExhaustFeature> _plugin;

} // namespace landmarks
