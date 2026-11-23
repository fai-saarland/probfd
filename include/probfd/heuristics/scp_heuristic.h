#ifndef PROBFD_HEURISTICS_SCP_HEURISTIC_H
#define PROBFD_HEURISTICS_SCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include "probfd/cost_partitioning/pattern_collection_sorter_factory.h"

#include <memory>

// Forward Declarations
namespace downward::utils {
class RandomNumberGenerator;
} // namespace downward::utils

namespace probfd::pdbs {
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class SCPHeuristicFactory final : public TaskHeuristicFactory {
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;
    const std::shared_ptr<cost_partitioning::PatternCollectionSorterFactory>
        sorter_factory_;

public:
    SCPHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator>
            pattern_collection_generator,
        std::shared_ptr<cost_partitioning::PatternCollectionSorterFactory>
            sorter_factory);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_SCP_HEURISTIC_H
