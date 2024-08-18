#include <utility>

#include "probfd/pdbs/pattern_collection_generator_classical.h"

#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/subcollection_finder_factory.h"

#include "probfd/tasks/all_outcomes_determinization.h"

#include "downward/pdbs/pattern_generator.h"

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

} // namespace probfd::pdbs
