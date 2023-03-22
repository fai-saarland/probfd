#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace occupation_measures {

static PluginTypePlugin<ConstraintGenerator>
    _type_plugin("OMConstraintGenerator", "");

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd