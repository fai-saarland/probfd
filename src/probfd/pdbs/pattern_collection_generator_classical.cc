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
    const SharedProbabilisticTask& task)
{
    auto determinization = tasks::create_determinization_task(task);
    auto finder = finder_factory_->create_subcollection_finder(task);

    return PatternCollectionInformation(
        task,
        gen_->generate(determinization),
        std::move(finder));
}

} // namespace probfd::pdbs
