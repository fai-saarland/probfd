#include "pattern_generator.h"

#include "../../../../../plugin.h"

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace pattern_selection {

static PluginTypePlugin<PatternCollectionGenerator> _type_plugin_collection(
    "PatternCollectionGenerator_mp",
    "Factory for pattern collections in the MaxProb setting");

} // namespace pattern_selection
} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic