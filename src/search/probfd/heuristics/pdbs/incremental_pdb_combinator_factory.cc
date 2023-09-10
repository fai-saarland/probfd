#include "probfd/heuristics/pdbs/incremental_pdb_combinator_factory.h"

#include "probfd/heuristics/pdbs/incremental_pdb_combinator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<PDBCombinator>
IncrementalPDBCombinatorFactory::create_pdb_combinator(
    ProbabilisticTaskProxy task_proxy,
    PPDBCollection& pdbs)
{
    auto combinator = this->create_incremental_pdb_combinator(task_proxy);
    combinator->update(pdbs);
    return combinator;
}

static class IncrementalPDBCombinatorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<IncrementalPDBCombinatorFactory> {
public:
    IncrementalPDBCombinatorFactoryCategoryPlugin()
        : TypedCategoryPlugin("IncrementalPDBCombinatorFactory")
    {
    }
} _;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd