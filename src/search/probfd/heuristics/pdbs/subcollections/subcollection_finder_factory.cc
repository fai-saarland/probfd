#include "probfd/heuristics/pdbs/subcollections/subcollection_finder_factory.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

static PluginTypePlugin<SubCollectionFinderFactory>
    _plugin("SubCollectionFinderFactory", "");

} // namespace pdbs
} // namespace heuristics
} // namespace probfd