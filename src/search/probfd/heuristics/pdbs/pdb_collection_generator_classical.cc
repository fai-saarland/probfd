#include "probfd/heuristics/pdbs/pdb_collection_generator_classical.h"

#include "probfd/heuristics/pdbs/pdb_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

#include "probfd/tasks/all_outcomes_determinization.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/tasks/root_task.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBCollectionGeneratorClassical::PDBCollectionGeneratorClassical(
    const utils::LogProxy& log,
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
    std::shared_ptr<SubCollectionFinderFactory> finder_factory)
    : PDBCollectionGenerator(log)
    , gen(gen)
    , finder_factory(finder_factory)
{
}

PDBCollectionGeneratorClassical::PDBCollectionGeneratorClassical(
    const plugins::Options& opts)
    : PDBCollectionGeneratorClassical(
          utils::get_log_from_options(opts),
          opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
              "generator"),
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"))
{
}

PDBCollectionInformation PDBCollectionGeneratorClassical::generate(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    ProbabilisticTaskProxy task_proxy(*task);

    std::shared_ptr<tasks::AODDeterminizationTask> determinization(
        new tasks::AODDeterminizationTask(task.get()));

    auto pattern_collection_info = gen->generate(determinization);

    utils::Timer timer;

    std::cout << "Computing PDBs for pattern collection..." << std::endl;
    PPDBCollection pdbs;

    if (pattern_collection_info.has_pdbs()) {
        for (const auto& dpdb : *pattern_collection_info.get_pdbs()) {
            pdbs.emplace_back(new ProbabilityAwarePatternDatabase(
                task_proxy,
                *task_cost_function,
                *dpdb,
                task_proxy.get_initial_state()));
        }
    } else {
        for (const Pattern& pattern : *pattern_collection_info.get_patterns()) {
            pdbs.emplace_back(new ProbabilityAwarePatternDatabase(
                task_proxy,
                *task_cost_function,
                pattern,
                task_proxy.get_initial_state()));
        }
    }
    std::cout << "Done computing PDBs for pattern collection: " << timer
              << std::endl;

    return PDBCollectionInformation(
        std::move(pdbs),
        finder_factory->create_subcollection_finder(task_proxy));
}

class PDBCollectionGeneratorClassicalFeature
    : public plugins::TypedFeature<
          PDBCollectionGenerator,
          PDBCollectionGeneratorClassical> {
public:
    PDBCollectionGeneratorClassicalFeature()
        : TypedFeature("classical_generator")
    {
        document_title("Classical PDB Generation Adapter");
        document_synopsis(
            "Uses a classical PDB generation method on the determinization "
            "of the input task. If classical PDBs are constructed by the "
            "generation algorithm, they are used as a heuristic to compute the "
            "corresponding probability-aware PDBs.");

        add_pdb_collection_generator_options_to_feature(*this);

        add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
            "generator",
            "The classical pattern collection generator.",
            "systematic()");

        add_option<std::shared_ptr<SubCollectionFinderFactory>>(
            "subcollection_finder_factory",
            "The subcollection finder factory.",
            "finder_trivial_factory()");
    }
};

static plugins::FeaturePlugin<PDBCollectionGeneratorClassicalFeature> _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd