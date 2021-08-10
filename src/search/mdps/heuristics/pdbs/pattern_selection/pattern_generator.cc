#include "pattern_generator.h"

#include "../../../../plugin.h"

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <class PDBType>
std::shared_ptr<utils::Printable>
PatternCollectionGenerator<PDBType>::get_report() const
{
    return nullptr;
};

template class PatternCollectionGenerator<expected_cost::ExpCostProjection>;
template class PatternCollectionGenerator<maxprob::MaxProbProjection>;

static PluginTypePlugin<
    PatternCollectionGenerator<expected_cost::ExpCostProjection>>
    _type_plugin_collection_ec(
        "PatternCollectionGenerator_ec",
        "Factory for pattern collections in the expected-cost setting");

static PluginTypePlugin<PatternCollectionGenerator<maxprob::MaxProbProjection>>
    _type_plugin_collection_mp(
        "PatternCollectionGenerator_mp",
        "Factory for pattern collections in the MaxProb setting");

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic