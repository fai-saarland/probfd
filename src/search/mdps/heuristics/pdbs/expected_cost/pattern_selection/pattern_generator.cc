#include "pattern_generator.h"

#include "../../../../../plugin.h"

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

static PluginTypePlugin<PatternCollectionGenerator> _type_plugin_collection(
    "PatternCollectionGenerator_ec",
    "Factory for pattern collections in the expected-cost setting");

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic