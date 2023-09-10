#include "probfd/heuristics/pdbs/additive_combinator_factory.h"
#include "probfd/heuristics/pdbs/additive_combinator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<PDBCombinator> AdditiveCombinatorFactory::create_pdb_combinator(
    ProbabilisticTaskProxy,
    PPDBCollection& pdbs)
{
    return std::make_unique<AdditiveCombinator>();
}

class AdditiveCombinatorFactoryFeature
    : public plugins::
          TypedFeature<PDBCombinatorFactory, AdditiveCombinatorFactory> {
public:
    AdditiveCombinatorFactoryFeature()
        : TypedFeature("additive_combinator_factory")
    {
    }

    std::shared_ptr<AdditiveCombinatorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<AdditiveCombinatorFactory>();
    }
};

static plugins::FeaturePlugin<AdditiveCombinatorFactoryFeature> _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd