#ifndef PROBFD_HEURISTICS_OCP_HEURISTIC_H
#define PROBFD_HEURISTICS_OCP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include <memory>

// Forward Declarations
namespace downward::lp {
enum class LPSolverType;
}

namespace probfd::pdbs {
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class OCPHeuristicFactory final : public TaskHeuristicFactory {
    const downward::lp::LPSolverType solver_type;

    const std::shared_ptr<pdbs::PatternCollectionGenerator>
        pattern_collection_generator_;

public:
    explicit OCPHeuristicFactory(
        downward::lp::LPSolverType solver_type,
        std::shared_ptr<pdbs::PatternCollectionGenerator>
            pattern_collection_generator);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_SCP_HEURISTIC_H
