#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

#include "downward/landmarks/landmark_factory.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;

namespace downward::cli::landmarks {

std::size_t add_landmark_factory_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    return add_log_options_to_feature(feature, start_index);
}

std::size_t
add_use_orders_option_to_feature(InternalFunctionDefinitionBase& feature, std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "use_orders",
        "true",
        "use orders between landmarks");

    return 1;
}

} // namespace downward::cli::landmarks
