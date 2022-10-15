#include "probfd/heuristics/pdbs/pattern_selection/pattern_generator.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <class PDBType>
std::shared_ptr<utils::Printable>
PatternCollectionGenerator<PDBType>::get_report() const
{
    return nullptr;
};

template class PatternCollectionGenerator<ExpCostProjection>;
template class PatternCollectionGenerator<MaxProbProjection>;

static PluginTypePlugin<PatternCollectionGenerator<ExpCostProjection>>
    _type_plugin_collection_ec(
        "PatternCollectionGenerator_ec",
        "Factory for pattern collections in the expected-cost setting");

static PluginTypePlugin<PatternCollectionGenerator<MaxProbProjection>>
    _type_plugin_collection_mp(
        "PatternCollectionGenerator_mp",
        "Factory for pattern collections in the MaxProb setting");

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd