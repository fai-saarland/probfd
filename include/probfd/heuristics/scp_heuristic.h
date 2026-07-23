#ifndef PROBFD_HEURISTICS_SCP_HEURISTIC_H
#define PROBFD_HEURISTICS_SCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

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
public:
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

private:
    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;
    const OrderingStrategy ordering_;
    const int random_seed_;

public:
    SCPHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator>
            pattern_collection_generator,
        OrderingStrategy ordering,
        int random_seed);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_SCP_HEURISTIC_H
