#include "downward_cli/landmarks/landmark_factory_rpg_exhaust_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_exhaust.h"

using namespace std;
using namespace downward::utils;

using namespace language;
using namespace language::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;
using downward::cli::landmarks::get_landmark_factory_arguments_from_options;

using namespace downward::landmarks;

namespace {
class LandmarkFactoryRpgExhaustFeature : public TypedFeature<LandmarkFactory> {
public:
    LandmarkFactoryRpgExhaustFeature()
        : TypedFeature("lm_exhaust")
    {
        document_title("Exhaustive Landmarks");
        document_synopsis(
            "Exhaustively checks for each fact if it is a landmark."
            "This check is done using relaxed planning.");

        add_option<bool>(
            "use_unary_relaxation",
            "compute landmarks of the unary relaxation, i.e., landmarks "
            "for the delete relaxation of a task transformation such that each "
            "operator is split into one operator for each of its effects. This "
            "kind of landmark was previously known as \"causal landmarks\". "
            "Setting the option to true can reduce the overall number of "
            "landmarks, which can make the search more memory-efficient but "
            "potentially less informative.",
            "false");
        add_landmark_factory_options_to_feature(*this);

        document_language_support(
            "conditional_effects",
            "ignored, i.e. not supported");
    }

    shared_ptr<LandmarkFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<LandmarkFactoryRpgExhaust>(
            opts.get<bool>(context, "use_unary_relaxation"),
            get_landmark_factory_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_rpg_exhaust_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<LandmarkFactoryRpgExhaustFeature>();
}

} // namespace downward::cli::landmarks
