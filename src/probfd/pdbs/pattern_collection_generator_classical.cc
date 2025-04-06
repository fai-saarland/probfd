#include <utility>

#include "probfd/pdbs/pattern_collection_generator_classical.h"

#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/subcollection_finder_factory.h"

#include "probfd/tasks/determinization_task.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward;

namespace probfd::pdbs {

PatternCollectionGeneratorClassical::PatternCollectionGeneratorClassical(
    std::shared_ptr<downward::pdbs::PatternCollectionGenerator> gen,
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
    const ProbabilisticTaskProxy task_proxy(*task);

    auto determinization = std::make_shared<tasks::DeterminizationTask>(task);
    auto finder = finder_factory_->create_subcollection_finder(task_proxy);

    return PatternCollectionInformation(
        task_proxy,
        task_cost_function,
        gen_->generate(determinization),
        std::move(finder));
}

} // namespace probfd::pdbs
