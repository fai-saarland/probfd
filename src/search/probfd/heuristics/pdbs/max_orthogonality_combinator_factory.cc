#include "probfd/heuristics/pdbs/max_orthogonality_combinator_factory.h"
#include "probfd/heuristics/pdbs/max_orthogonality_combinator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

MaxOrthogonalityCombinatorFactory::MaxOrthogonalityCombinatorFactory(
    const plugins::Options& opts)
    : multiplicative(opts.get<bool>("multiplicative"))
{
}

MaxOrthogonalityCombinatorFactory::MaxOrthogonalityCombinatorFactory(
    bool multiplicative)
    : multiplicative(multiplicative)
{
}

std::unique_ptr<IncrementalPDBCombinator>
MaxOrthogonalityCombinatorFactory::create_incremental_pdb_combinator(
    ProbabilisticTaskProxy task_proxy)
{
    if (multiplicative) {
        return std::make_unique<MultiplicativeMaxOrthogonalityCombinator>(
            task_proxy);
    }
    return std::make_unique<AdditiveMaxOrthogonalityCombinator>(task_proxy);
}

class MaxOrthogonalityCombinatorFactoryFeature
    : public plugins::TypedFeature<
          PDBCombinatorFactory,
          MaxOrthogonalityCombinatorFactory> {
public:
    MaxOrthogonalityCombinatorFactoryFeature()
        : TypedFeature("max_orthogonality_factory")
    {
        add_option<bool>(
            "multiplicative",
            "whether pdb estimates shall be multiplied instead of added "
            "(MaxProb analysis)",
            "false");
    }
};

static plugins::FeaturePlugin<MaxOrthogonalityCombinatorFactoryFeature> _;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd