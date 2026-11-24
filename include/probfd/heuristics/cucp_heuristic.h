#ifndef PROBFD_HEURISTICS_CUCP_HEURISTIC_H
#define PROBFD_HEURISTICS_CUCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include <memory>

namespace probfd::pdbs {
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class CUCPHeuristicFactory final : public TaskHeuristicFactory {
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;

public:
    explicit CUCPHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif
