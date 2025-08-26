#include "downward/cli/landmarks/landmark_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/landmarks/landmark_factory.h"

using namespace downward::landmarks;

using namespace downward::cli::plugins;

namespace downward::cli::landmarks {

void add_landmark_factory_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<LandmarkFactory>("LandmarkFactory");
    category.document_synopsis(
        "A landmark factory specification is either a newly created "
        "instance or a landmark factory that has been defined previously. "
        "This page describes how one can specify a new landmark factory "
        "instance. For re-using landmark factories, see "
        "OptionSyntax#Landmark_Predefinitions.");
}

} // namespace downward::cli::landmarks
