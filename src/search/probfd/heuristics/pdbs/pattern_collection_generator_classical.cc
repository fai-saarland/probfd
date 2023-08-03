#include "probfd/heuristics/pdbs/pattern_collection_generator_classical.h"

#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

#include "probfd/tasks/all_outcomes_determinization.h"

#include "probfd/cost_model.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/tasks/root_task.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PatternCollectionGeneratorClassical::PatternCollectionGeneratorClassical(
    const utils::LogProxy& log,
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
    std::shared_ptr<SubCollectionFinderFactory> finder_factory)
    : PatternCollectionGenerator(log)
    , gen(gen)
    , finder_factory(finder_factory)
{
}

PatternCollectionGeneratorClassical::PatternCollectionGeneratorClassical(
    const plugins::Options& opts)
    : PatternCollectionGeneratorClassical(
          utils::get_log_from_options(opts),
          opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
              "generator"),
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"))
{
}

PatternCollectionInformation PatternCollectionGeneratorClassical::generate(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    ProbabilisticTaskProxy task_proxy(*task);
    TaskSimpleCostFunction* task_cost_function =
        g_cost_model->get_cost_function();

    std::shared_ptr<tasks::AODDeterminizationTask> determinization(
        new tasks::AODDeterminizationTask(task.get()));

    std::shared_ptr<SubCollectionFinder> finder =
        finder_factory->create_subcollection_finder(task_proxy);
    return PatternCollectionInformation(
        ProbabilisticTaskProxy(*task),
        task_cost_function,
        gen->generate(determinization),
        finder);
}

class PatternCollectionGeneratorClassicalFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorClassical> {
public:
    PatternCollectionGeneratorClassicalFeature()
        : TypedFeature("classical_generator")
    {
        document_title("Classical Pattern Generation Adapter");
        document_synopsis(
            "Uses a classical pattern generation method on the determinization "
            "of the input task. If classical PDBs are constructed by the "
            "generation algorithm, they are used as a heuristic to compute the "
            "corresponding probability-aware PDBs.");

        add_pattern_collection_generator_options_to_feature(*this);

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

static plugins::FeaturePlugin<PatternCollectionGeneratorClassicalFeature>
    _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd