#include "probfd/heuristics/pdbs/pattern_collection_from_pdbs_generator.h"

#include "probfd/heuristics/pdbs/pdb_collection_generator.h"
#include "probfd/heuristics/pdbs/pdb_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "downward/plugins/plugin.h"
#include "downward/plugins/plugin_info.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

PatternCollectionFromPDBsGenerator::PatternCollectionFromPDBsGenerator(
    const plugins::Options& opts)
    : PatternCollectionGenerator(std::move(utils::get_log_from_options(opts)))
    , pdb_collection_generator(
          opts.get<std::shared_ptr<PDBCollectionGenerator>>("generator"))
{
}

PatternCollectionFromPDBsGenerator::PatternCollectionFromPDBsGenerator(
    utils::LogProxy log,
    std::shared_ptr<PDBCollectionGenerator> pdb_collection_generator)
    : PatternCollectionGenerator(std::move(log))
    , pdb_collection_generator(std::move(pdb_collection_generator))
{
}

std::shared_ptr<PatternCollection>
PatternCollectionFromPDBsGenerator::generate_pattern_collection(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    auto info = pdb_collection_generator->generate(task, task_cost_function);

    auto patterns = std::make_shared<PatternCollection>();

    for (const auto& pdb : info.get_pdbs()) {
        patterns->push_back(pdb->get_pattern());
    }

    return patterns;
}

class PatternCollectionFromPDBsGeneratorFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionFromPDBsGenerator> {
public:
    PatternCollectionFromPDBsGeneratorFeature()
        : TypedFeature("from_pdb_generator")
    {
        add_pattern_collection_generator_options_to_feature(*this);
        add_option<std::shared_ptr<PDBCollectionGenerator>>(
            "generator",
            "The PDB generator.",
            plugins::ArgumentInfo::NO_DEFAULT);
    }
};

static plugins::FeaturePlugin<PatternCollectionFromPDBsGeneratorFeature> _;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
