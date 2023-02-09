#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

template <class PDBType>
std::shared_ptr<utils::Printable>
PatternCollectionGenerator<PDBType>::get_report() const
{
    return nullptr;
};

template class PatternCollectionGenerator<SSPPatternDatabase>;
template class PatternCollectionGenerator<MaxProbPatternDatabase>;

static PluginTypePlugin<PatternCollectionGenerator<SSPPatternDatabase>>
    _type_plugin_collection_ec(
        "PatternCollectionGenerator_ec",
        "Factory for pattern collections in the expected-cost setting");

static PluginTypePlugin<PatternCollectionGenerator<MaxProbPatternDatabase>>
    _type_plugin_collection_mp(
        "PatternCollectionGenerator_mp",
        "Factory for pattern collections in the MaxProb setting");

} // namespace pdbs
} // namespace heuristics
} // namespace probfd