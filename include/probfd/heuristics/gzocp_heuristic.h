#ifndef PROBFD_HEURISTICS_GZOCP_HEURISTIC_H
#define PROBFD_HEURISTICS_GZOCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace downward::utils {
class RandomNumberGenerator;
} // namespace downward::utils

namespace probfd::pdbs {
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class GZOCPHeuristicFactory final : public TaskHeuristicFactory {
public:
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

private:
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;
    const OrderingStrategy ordering_;
    const std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit GZOCPHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator>
            pattern_collection_generator,
        OrderingStrategy ordering,
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_GZOCP_HEURISTIC_H
