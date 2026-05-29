#ifndef PROBFD_HEURISTICS_UCP_HEURISTIC_H
#define PROBFD_HEURISTICS_UCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

namespace probfd::pdbs {
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class UCPHeuristicFactory final : public TaskHeuristicFactory {
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;

public:
    explicit UCPHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_UCP_HEURISTIC_H
