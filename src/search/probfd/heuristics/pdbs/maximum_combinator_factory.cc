#include "probfd/heuristics/pdbs/maximum_combinator_factory.h"
#include "probfd/heuristics/pdbs/maximum_combinator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<IncrementalPDBCombinator>
MaximumCombinatorFactory::create_incremental_pdb_combinator(
    ProbabilisticTaskProxy)
{
    return std::make_unique<MaximumCombinator>();
}

class MaximumCombinatorFactoryFeature
    : public plugins::
          TypedFeature<PDBCombinatorFactory, MaximumCombinatorFactory> {
public:
    MaximumCombinatorFactoryFeature()
        : TypedFeature("maximum_combinator_factory")
    {
    }

    std::shared_ptr<MaximumCombinatorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<MaximumCombinatorFactory>();
    }
};

static plugins::FeaturePlugin<MaximumCombinatorFactoryFeature> _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd