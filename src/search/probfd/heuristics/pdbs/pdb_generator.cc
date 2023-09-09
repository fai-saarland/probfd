#include "probfd/heuristics/pdbs/pdb_generator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBGenerator::PDBGenerator(const plugins::Options& opts)
    : log(utils::get_log_from_options(opts))
{
}

PDBGenerator::PDBGenerator(const utils::LogProxy& log)
    : log(log)
{
}

void add_pattern_generator_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

static class PDBGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PDBGenerator> {
public:
    PDBGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PAPDBGenerator")
    {
        document_synopsis("Factory for a probability-aware PDB");
    }
} _category_plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd