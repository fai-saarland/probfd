#include "probfd/heuristics/pdbs/pdb_collection_generator.h"

#include "probfd/heuristics/pdbs/pdb_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBCollectionGenerator::PDBCollectionGenerator(const plugins::Options& opts)
    : log(utils::get_log_from_options(opts))
{
}

PDBCollectionGenerator::PDBCollectionGenerator(utils::LogProxy log)
    : log(std::move(log))
{
}

void add_pdb_collection_generator_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

namespace {
static class PDBCollectionGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PDBCollectionGenerator> {
public:
    PDBCollectionGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PAPDBCollectionGenerator")
    {
        document_synopsis("Factory for probability-aware PDB collections");
    }
} _category_plugin;
} // namespace

} // namespace pdbs
} // namespace heuristics
} // namespace probfd