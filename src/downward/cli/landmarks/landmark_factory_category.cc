#include "downward/cli/landmarks/landmark_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/landmarks/landmark_factory.h"

using namespace downward::landmarks;

using namespace language::plugins;

namespace downward::cli::landmarks {

void add_landmark_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<LandmarkFactory>(
        "LandmarkFactory",
        "A landmark factory specification is either a newly created "
        "instance or a landmark factory that has been defined previously. "
        "This page describes how one can specify a new landmark factory "
        "instance. For re-using landmark factories, see "
        "OptionSyntax#Landmark_Predefinitions.");
}

} // namespace downward::cli::landmarks
