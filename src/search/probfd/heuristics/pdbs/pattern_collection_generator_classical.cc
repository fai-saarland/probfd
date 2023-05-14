#include "probfd/heuristics/pdbs/pattern_collection_generator_classical.h"

#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

#include "probfd/cost_model.h"

#include "options/options.h"

#include "pdbs/pattern_database.h"

#include "option_parser.h"
#include "plugin.h"

#include "probfd/tasks/all_outcomes_determinization.h"
#include "tasks/root_task.h"

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
    const options::Options& opts)
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
    TaskCostFunction* task_cost_function = g_cost_model->get_cost_function();

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

static std::shared_ptr<PatternCollectionGeneratorClassical>
_parse(OptionParser& parser)
{
    parser.document_synopsis(
        "Classical Pattern Generation Adapter",
        "Uses a classical pattern generation method on the determinization of "
        "the input task. If classical PDBs are constructed by the generation "
        "algorithm, they are used as a heuristic to compute the corresponding "
        "probability-aware PDBs.");

    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "generator",
        "The classical pattern collection generator.",
        "systematic()");

    parser.add_option<std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "The subcollection finder factory.",
        "finder_trivial_factory()");

    add_generator_options_to_parser(parser);

    if (parser.dry_run()) return nullptr;

    Options opts = parser.parse();
    return std::make_shared<PatternCollectionGeneratorClassical>(opts);
}

static Plugin<PatternCollectionGenerator>
    _plugin("classical_generator", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd