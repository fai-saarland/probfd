#include <utility>

#include "probfd/pdbs/pattern_collection_generator_classical.h"

#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/subcollection_finder_factory.h"

#include "probfd/tasks/all_outcomes_determinization.h"

#include "downward/pdbs/pattern_generator.h"

#include "downward/plugins/plugin.h"

namespace probfd::pdbs {

PatternCollectionGeneratorClassical::PatternCollectionGeneratorClassical(
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
    std::shared_ptr<SubCollectionFinderFactory> finder_factory,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , gen_(std::move(gen))
    , finder_factory_(std::move(finder_factory))
{
}

PatternCollectionInformation PatternCollectionGeneratorClassical::generate(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    ProbabilisticTaskProxy task_proxy(*task);

    std::shared_ptr<tasks::AODDeterminizationTask> determinization(
        new tasks::AODDeterminizationTask(task.get()));

    std::shared_ptr<SubCollectionFinder> finder =
        finder_factory_->create_subcollection_finder(task_proxy);
    return PatternCollectionInformation(
        task_proxy,
        task_cost_function,
        gen_->generate(determinization),
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

    std::shared_ptr<PatternCollectionGeneratorClassical>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            PatternCollectionGeneratorClassical>(
            opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
                "generator"),
            opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
                "subcollection_finder_factory"),
            utils::get_log_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<PatternCollectionGeneratorClassicalFeature>
    _plugin;

} // namespace probfd::pdbs
