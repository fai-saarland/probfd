#include "downward_plugins/landmarks/landmark_factory_options.h"

#include "downward_plugins/utils/logging_options.h"

#include "downward_plugins/plugins/plugin.h"

#include "downward/landmarks/landmark_factory.h"

using namespace std;
using namespace landmarks;
using namespace utils;

using namespace downward_plugins::plugins;

using downward_plugins::utils::add_log_options_to_feature;
using downward_plugins::utils::get_log_arguments_from_options;

namespace downward_plugins::landmarks {

void add_landmark_factory_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

tuple<Verbosity>
get_landmark_factory_arguments_from_options(const Options& opts)
{
    return get_log_arguments_from_options(opts);
}

void add_use_orders_option_to_feature(Feature& feature)
{
    feature.add_option<bool>(
        "use_orders",
        "use orders between landmarks",
        "true");
}

bool get_use_orders_arguments_from_options(const Options& opts)
{
    return opts.get<bool>("use_orders");
}

void add_only_causal_landmarks_option_to_feature(Feature& feature)
{
    feature.add_option<bool>(
        "only_causal_landmarks",
        "keep only causal landmarks",
        "false");
}

bool get_only_causal_landmarks_arguments_from_options(const Options& opts)
{
    return opts.get<bool>("only_causal_landmarks");
}

class LandmarkFactoryCategoryPlugin
    : public TypedCategoryPlugin<LandmarkFactory> {
public:
    LandmarkFactoryCategoryPlugin()
        : TypedCategoryPlugin("LandmarkFactory")
    {
        document_synopsis(
            "A landmark factory specification is either a newly created "
            "instance or a landmark factory that has been defined previously. "
            "This page describes how one can specify a new landmark factory "
            "instance. For re-using landmark factories, see "
            "OptionSyntax#Landmark_Predefinitions.");
        allow_variable_binding();
    }
} _category_plugin;

} // namespace downward_plugins::landmarks
