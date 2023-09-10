#include "probfd/heuristics/pdbs/pdb_combinator_factory.h"

#include "probfd/heuristics/pdbs/pdb_combinator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

static class PDBCombinatorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<PDBCombinatorFactory> {
public:
    PDBCombinatorFactoryCategoryPlugin()
        : TypedCategoryPlugin("PDBCombinatorFactory")
    {
    }
} _category_plugin_collection;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd